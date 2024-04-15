#include <SPI.h>
#include <DW1000.h>

// connection pins
const uint8_t PIN_RST = 9; // reset pin
const uint8_t PIN_IRQ = 2; // irq pin
const uint8_t PIN_SS = SS; // spi select pin

// messages used in the ranging protocol
// TODO replace by enum
#define POLL 0
#define POLL_ACK 1
#define RANGE 2
#define RANGE_REPORT 3
#define RANGE_FAILED 255
#define LEN_DATA 16
#define DETECTION_RADIUS 5

#define SLEEP_DEBUG 3
#define TIMER_DEBUG 4
#define WAITING_DEBUG 5
#define ALARM_DEBUG 6
#define BUTTON_PIN 7

#define PRESSURE_DEBUG A5

// message flow state
volatile byte expectedMsgId = POLL;
// message sent/received state
volatile boolean sentAck = false;
volatile boolean receivedAck = false;
// protocol error state
boolean protocolFailed = false;
// timestamps to remember
DW1000Time timePollSent;
DW1000Time timePollReceived;
DW1000Time timePollAckSent;
DW1000Time timePollAckReceived;
DW1000Time timeRangeSent;
DW1000Time timeRangeReceived;
// last computed range/time
DW1000Time timeComputedRange;
// data buffer
byte data[LEN_DATA];
// watchdog and reset period
uint32_t lastActivity;
uint32_t resetPeriod = 250;
// reply times (same on both sides for symm. ranging)
uint16_t replyDelayTimeUS = 3000;
// ranging counter (per second)
uint16_t successRangingCount = 0;
uint32_t rangingCountPeriod = 0;
float samplingRate = 0;

// Threshold for the force resistor's reading to switch from SLEEP to TIMER state
unsigned long time_keys_left = 0;
unsigned long transceiver_timer = 0;

// Define an enum for the system states
enum SystemState {
  SLEEP,
  TIMER,
  WAITING,
  ALARM
};

// Global variable to hold the current state of the system
SystemState systemState = SLEEP;

void setup() {
    transceiverSetup();
  
  // // Set the transceiver CS pin as an output:
  // pinMode(TRANSCEIVER_CS_PIN, OUTPUT);

  // // Set the CS pin high to start with the transceiver not selected
  // digitalWrite(TRANSCEIVER_CS_PIN, HIGH);
  
  // Configure the button pin as an input with an internal pull-up resistor
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // pinMode(SPEAKER_PIN, OUTPUT);

  pinMode(SLEEP_DEBUG, OUTPUT);
  pinMode(TIMER_DEBUG, OUTPUT);
  pinMode(WAITING_DEBUG, OUTPUT);
  pinMode(ALARM_DEBUG, OUTPUT);

  pinMode(PRESSURE_DEBUG, INPUT_PULLUP);

}

void transceiverSetup(){
  // DEBUG monitoring
    Serial.begin(115200);
    delay(1000);
    Serial.println(F("### DW1000-arduino-ranging-anchor ###"));
    // initialize the driver
    DW1000.begin(PIN_IRQ, PIN_RST);
    DW1000.select(PIN_SS);
    Serial.println(F("DW1000 initialized ..."));
    // general configuration
    DW1000.newConfiguration();
    DW1000.setDefaults();
    DW1000.setDeviceAddress(1);
    DW1000.setNetworkId(10);
    DW1000.enableMode(DW1000.MODE_LONGDATA_RANGE_LOWPOWER);
    DW1000.commitConfiguration();
    Serial.println(F("Committed configuration ..."));
    // DEBUG chip info and registers pretty printed
    char msg[128];
    DW1000.getPrintableDeviceIdentifier(msg);
    Serial.print("Device ID: "); Serial.println(msg);
    DW1000.getPrintableExtendedUniqueIdentifier(msg);
    Serial.print("Unique ID: "); Serial.println(msg);
    DW1000.getPrintableNetworkIdAndShortAddress(msg);
    Serial.print("Network ID & Device Address: "); Serial.println(msg);
    DW1000.getPrintableDeviceMode(msg);
    Serial.print("Device mode: "); Serial.println(msg);
    // attach callback for (successfully) sent and received messages
    DW1000.attachSentHandler(handleSent);
    DW1000.attachReceivedHandler(handleReceived);
    // anchor starts in receiving mode, awaiting a ranging poll message
    receiver();
    noteActivity();
    // for first time ranging frequency computation
    rangingCountPeriod = millis();
}

// RANGING HELPER FUNCTIONS

void noteActivity() {
    // update activity timestamp, so that we do not reach "resetPeriod"
    lastActivity = millis();
}

void resetInactive() {
    // anchor listens for POLL
    expectedMsgId = POLL;
    receiver();
    noteActivity();
}

void handleSent() {
    // status change on sent success
    sentAck = true;
}

void handleReceived() {
    // status change on received success
    receivedAck = true;
}

void transmitPollAck() {
    DW1000.newTransmit();
    DW1000.setDefaults();
    data[0] = POLL_ACK;
    // delay the same amount as ranging tag
    DW1000Time deltaTime = DW1000Time(replyDelayTimeUS, DW1000Time::MICROSECONDS);
    DW1000.setDelay(deltaTime);
    DW1000.setData(data, LEN_DATA);
    DW1000.startTransmit();
}

void transmitRangeReport(float curRange) {
    DW1000.newTransmit();
    DW1000.setDefaults();
    data[0] = RANGE_REPORT;
    // write final ranging result
    memcpy(data + 1, &curRange, 4);
    DW1000.setData(data, LEN_DATA);
    DW1000.startTransmit();
}

void transmitRangeFailed() {
    DW1000.newTransmit();
    DW1000.setDefaults();
    data[0] = RANGE_FAILED;
    DW1000.setData(data, LEN_DATA);
    DW1000.startTransmit();
}

void receiver() {
    DW1000.newReceive();
    DW1000.setDefaults();
    // so we don't need to restart the receiver manually
    DW1000.receivePermanently(true);
    DW1000.startReceive();
}

/*
 * RANGING ALGORITHMS
 * ------------------
 * Either of the below functions can be used for range computation (see line "CHOSEN
 * RANGING ALGORITHM" in the code).
 * - Asymmetric is more computation intense but least error prone
 * - Symmetric is less computation intense but more error prone to clock drifts
 *
 * The anchors and tags of this reference example use the same reply delay times, hence
 * are capable of symmetric ranging (and of asymmetric ranging anyway).
 */

void computeRangeAsymmetric() {
    // asymmetric two-way ranging (more computation intense, less error prone)
    DW1000Time round1 = (timePollAckReceived - timePollSent).wrap();
    DW1000Time reply1 = (timePollAckSent - timePollReceived).wrap();
    DW1000Time round2 = (timeRangeReceived - timePollAckSent).wrap();
    DW1000Time reply2 = (timeRangeSent - timePollAckReceived).wrap();
    DW1000Time tof = (round1 * round2 - reply1 * reply2) / (round1 + round2 + reply1 + reply2);
    // set tof timestamp
    timeComputedRange.setTimestamp(tof);
}

void computeRangeSymmetric() {
    // symmetric two-way ranging (less computation intense, more error prone on clock drift)
    DW1000Time tof = ((timePollAckReceived - timePollSent) - (timePollAckSent - timePollReceived) +
                      (timeRangeReceived - timePollAckSent) - (timeRangeSent - timePollAckReceived)) * 0.25f;
    // set tof timestamp
    timeComputedRange.setTimestamp(tof);
}

/*
 * END RANGING ALGORITHMS
 * ----------------------
 */

// float pollKeychain(){
//   int32_t curMillis = millis();
//   float distance = -1.0;
//     if (!sentAck && !receivedAck) {
//         // check if inactive
//         if (curMillis - lastActivity > resetPeriod) {
//             resetInactive();
//         }
//     }
//     // continue on any success confirmation
//     if (sentAck) {
//         sentAck = false;
//         byte msgId = data[0];
//         if (msgId == POLL_ACK) {
//             DW1000.getTransmitTimestamp(timePollAckSent);
//             noteActivity();
//         }
//     }
//     if (receivedAck) {
//         receivedAck = false;
//         // get message and parse
//         DW1000.getData(data, LEN_DATA);
//         byte msgId = data[0];
//         if (msgId != expectedMsgId) {
//             // unexpected message, start over again (except if already POLL)
//             protocolFailed = true;
//         }
//         if (msgId == POLL) {
//             // on POLL we (re-)start, so no protocol failure
//             protocolFailed = false;
//             DW1000.getReceiveTimestamp(timePollReceived);
//             expectedMsgId = RANGE;
//             transmitPollAck();
//             noteActivity();
//         }
//         else if (msgId == RANGE) {
//             DW1000.getReceiveTimestamp(timeRangeReceived);
//             expectedMsgId = POLL;
//             if (!protocolFailed) {
//                 timePollSent.setTimestamp(data + 1);
//                 timePollAckReceived.setTimestamp(data + 6);
//                 timeRangeSent.setTimestamp(data + 11);
//                 // (re-)compute range as two-way ranging is done
//                 computeRangeAsymmetric(); // CHOSEN RANGING ALGORITHM
//                 transmitRangeReport(timeComputedRange.getAsMicroSeconds());
//                 distance = timeComputedRange.getAsMeters();
//                 Serial.print("Range: "); Serial.print(distance); Serial.print(" m");
//                 Serial.print("\t RX power: "); Serial.print(DW1000.getReceivePower()); Serial.print(" dBm");
//                 Serial.print("\t Sampling: "); Serial.print(samplingRate); Serial.println(" Hz");
//                 //Serial.print("FP power is [dBm]: "); Serial.print(DW1000.getFirstPathPower());
//                 //Serial.print("RX power is [dBm]: "); Serial.println(DW1000.getReceivePower());
//                 //Serial.print("Receive quality: "); Serial.println(DW1000.getReceiveQuality());
//                 // update sampling rate (each second)
//                 successRangingCount++;
//                 if (curMillis - rangingCountPeriod > 1000) {
//                     samplingRate = (1000.0f * successRangingCount) / (curMillis - rangingCountPeriod);
//                     rangingCountPeriod = curMillis;
//                     successRangingCount = 0;
//                 }
//             }
//             else {
//                 transmitRangeFailed();
//             }

//             noteActivity();
//         }
//     }

//     return distance;
// }

void transmitPoll() {
    DW1000.newTransmit();  // Prepare a new transmission
    DW1000.setDefaults();  // Set the default configuration
    data[0] = POLL;        // Set the first byte of the data buffer to the POLL message ID
    DW1000.setData(data, LEN_DATA);  // Load the data buffer into the DW1000
    DW1000.startTransmit();  // Start transmission
}


// Timeout for waiting a message in milliseconds
#define MESSAGE_TIMEOUT 250

float pollKeychain() {
    int32_t curMillis = millis();
    float distance = -1.0;

    // Start by sending a POLL message
    if (!sentAck && !receivedAck) {
        transmitPoll();  // Function to transmit a POLL message (needs implementation)
        lastActivity = curMillis;  // Update last activity time
    }

    // Wait for acknowledgment or timeout
    while ((millis() - lastActivity) < MESSAGE_TIMEOUT) {
        if (receivedAck) {
            break;  // Exit the loop if acknowledgment is received
        }
    }

    if ((millis() - lastActivity) >= MESSAGE_TIMEOUT) {
        Serial.println("bp1");
        return -1;  // Return -1 if the response was not received in time
    }

    // Process the received message if we have received an acknowledgment
    if (receivedAck) {
        receivedAck = false;  // Reset acknowledgment flag
        DW1000.getData(data, LEN_DATA);
        byte msgId = data[0];

        if (msgId == RANGE) {
            DW1000.getReceiveTimestamp(timeRangeReceived);
            expectedMsgId = POLL;  // Reset the expected message ID for the next cycle
            computeRangeAsymmetric();  // Compute the distance
            distance = timeComputedRange.getAsMeters();  // Convert the time-of-flight to distance in meters
        } else {
            transmitRangeFailed();  // Notify the failure in transmission
        }
    }

    noteActivity();  // Update the activity timestamp
    return distance;
}


void printState(SystemState state) {
    switch (state) {
        case SLEEP:
            Serial.println("SLEEP");
            break;
        case TIMER:
            Serial.println("TIMER");
            break;
        case WAITING:
            Serial.println("WAITING");
            break;
        case ALARM:
            Serial.println("ALARM");
            break;
        default:
            Serial.println("UNKNOWN");
            break;
    }
}



void loop() {

  unsigned long current_time = millis();
  // int pressureValue = analogRead(PRESSURE_SENSOR_PIN);

  // Read the button state
  bool buttonPressed = 0;

  // bool pressureRead = digitalRead(PRESSURE_DEBUG) == HIGH;
  bool pressureRead = analogRead(PRESSURE_DEBUG) > 100;
  float distance = pollKeychain();

  printState(systemState);

  // Serial.println("System is in: " + systemState);
  // Serial.println(analogRead(PRESSURE_DEBUG) > 100);


  switch (systemState) {
    case SLEEP:     
      // // Check if the pressure is below the threshold
      digitalWrite(SLEEP_DEBUG, HIGH);

      if(pressureRead){
        time_keys_left = millis();
        digitalWrite(SLEEP_DEBUG, LOW);
        systemState = TIMER;
        break;
      }

      break;
      
    case TIMER:
      // Perform actions for STATE_2
      digitalWrite(TIMER_DEBUG, HIGH);
      delay(5000);
      if(!pressureRead){
        digitalWrite(TIMER_DEBUG, LOW);
        systemState = SLEEP;
        break;
      }else{
        digitalWrite(TIMER_DEBUG, LOW);
        systemState = WAITING;
        break;
      }
      break;
    case WAITING:
      digitalWrite(WAITING_DEBUG, HIGH);
      // Serial.println("INSIDE WAITING STATE");
      Serial.println(distance);

      if (! pressureRead){
        digitalWrite(WAITING_DEBUG, LOW);
        systemState = SLEEP;
        break;
      }else if (distance != -1 && distance < DETECTION_RADIUS){
        delay(5000); // waits 5 seconds

        if (! pressureRead){ // if the keys were placed into the dish, switch to sleep
          digitalWrite(WAITING_DEBUG, LOW);
          systemState = SLEEP;
          break;
        }

        distance = pollKeychain(); // re read the distance
        Serial.println(distance);

        if (distance != -1 && distance < DETECTION_RADIUS){ // still in the vicinity after 5 seconds, switch to alarm state
          digitalWrite(WAITING_DEBUG, LOW);
          systemState = ALARM;
          break;
        }
        break;

        // the keychain has left the vicinity. go back to the waiting state
      }
      break;
    case ALARM:
      int test = analogRead(pressureRead);
      analogWrite(ALARM_DEBUG, 245); // Set the pin high
      // delay(30); // Wait for half the period (25ms for a 400Hz square wave)
      // analogWrite(ALARM_DEBUG, 1500); // Set the pin low
      // delay(30); // Wait for the other half of the period

      if(! pressureRead){
        digitalWrite(ALARM_DEBUG, LOW);
        systemState = SLEEP;
        break;
      }else if (buttonPressed){
        digitalWrite(ALARM_DEBUG, LOW);
        time_keys_left = millis();
        systemState = TIMER;
        break;
      }else if (distance != -1 && distance > DETECTION_RADIUS){
        digitalWrite(ALARM_DEBUG, LOW);
        systemState = WAITING;
        break;
      }
      break;
  }
  // Add any other state-independent actions here
  delay(100);
}
