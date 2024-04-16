#include <SPI.h>
#include "DW1000.h"
#include "DW1000CompileOptions.h"
#include "DW1000Constants.h"
#include "DW1000Device.h"
#include "DW1000Mac.h"
#include "DW1000Ranging.h"
#include "DW1000Time.h"
#include "require_cpp11.h"

#define DETECTION_RADIUS 5

#define SLEEP_DEBUG 3
#define TIMER_DEBUG 4
#define WAITING_DEBUG 5
#define ALARM_DEBUG 6
#define BUTTON_PIN 7

#define PRESSURE_DEBUG A5

// void newRange();
// void newBlink(DW1000Device* device);
// void inactiveDevice(DW1000Device* device);

// transcevier pins
// connection pins
const uint8_t PIN_RST = 9; // reset pin
const uint8_t PIN_IRQ = 2; // irq pin
const uint8_t PIN_SS = SS; // spi select pin

// Threshold for the force resistor's reading to switch from SLEEP to TIMER state
unsigned long timestamp = 0;
unsigned long transceiver_timer = 0;

bool inside_radius = false;
bool inside_radius_timer = false;


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
  // // Set the transceiver CS pin as an output:
  // pinMode(TRANSCEIVER_CS_PIN, OUTPUT);

  // // Set the CS pin high to start with the transceiver not selected
  // digitalWrite(TRANSCEIVER_CS_PIN, HIGH);
  transceiverSetup();
  
  // Configure the button pin as an input with an internal pull-up resistor
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // pinMode(SPEAKER_PIN, OUTPUT);

  pinMode(SLEEP_DEBUG, OUTPUT);
  pinMode(TIMER_DEBUG, OUTPUT);
  pinMode(WAITING_DEBUG, OUTPUT);
  pinMode(ALARM_DEBUG, OUTPUT);

  pinMode(PRESSURE_DEBUG, INPUT_PULLUP);

  Serial.println("Finished configuration");

}

void newRange() {
  Serial.print("from: "); Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
  Serial.print("\t Range: "); Serial.print(DW1000Ranging.getDistantDevice()->getRange()); Serial.print(" m");
  Serial.print("\t RX power: "); Serial.print(DW1000Ranging.getDistantDevice()->getRXPower()); Serial.println(" dBm");
}

void newBlink(DW1000Device* device) {
  Serial.print("blink; 1 device added ! -> ");
  Serial.print(" short:");
  Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device* device) {
  Serial.print("delete inactive device: ");
  Serial.println(device->getShortAddress(), HEX);
}

void transceiverSetup(){
  Serial.begin(115200);
  delay(1000);
  //init the configuration
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin
  //define the sketch as anchor. It will be great to dynamically change the type of module
  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachBlinkDevice(newBlink);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);
  //Enable the filter to smooth the distance
  //DW1000Ranging.useRangeFilter(true);
  
  //we start the module as an anchor
  DW1000Ranging.startAsAnchor("82:17:5B:D5:A9:9A:E2:9C", DW1000.MODE_LONGDATA_RANGE_ACCURACY);
  delay(1000);
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

  // Read the button state
  bool buttonPressed = digitalRead(BUTTON_PIN);

  // bool pressureRead = digitalRead(PRESSURE_DEBUG) == HIGH;
  bool pressureRead = analogRead(PRESSURE_DEBUG) > 100;

  float distance = DW1000Ranging.loop();
  Serial.println(distance);

  // printState(systemState);

  // Serial.println("System is in: " + systemState);
  // Serial.println(analogRead(PRESSURE_DEBUG) > 100);


  switch (systemState) {
    case SLEEP:     
      // // Check if the pressure is below the threshold
      digitalWrite(SLEEP_DEBUG, HIGH);

      if(pressureRead){
        timestamp = millis();
        digitalWrite(SLEEP_DEBUG, LOW);
        systemState = TIMER;
        break;
      }

      break;
      
    case TIMER:
      // Perform actions for STATE_2
      digitalWrite(TIMER_DEBUG, HIGH);

      // Barrier for the code to wait 5 seconds before continuing
      if(current_time - timestamp < 5000){
        break;
      }

      if(!pressureRead){
        digitalWrite(TIMER_DEBUG, LOW);
        systemState = SLEEP;
        break;
      }else{
        digitalWrite(TIMER_DEBUG, LOW);
        systemState = WAITING;
        timestamp = millis();
        break;
      }
      break;
    case WAITING:
      digitalWrite(WAITING_DEBUG, HIGH);
      // Serial.println("INSIDE WAITING STATE");
      // distance = pollKeychain();

      if (! pressureRead){
        digitalWrite(WAITING_DEBUG, LOW);
        systemState = SLEEP;
        break;
      }
      
      if (distance > 0 && distance < DETECTION_RADIUS || inside_radius){
        inside_radius == true;

        if (inside_radius_timer == false){
          inside_radius_timer = true;
          timestamp = millis();
          break;
        }
        
        // This is a barrier that allows the waiting state to wait X seconds after detecting the keys in the radius before switching to the alarm state
        if(current_time - timestamp < 5000){
          break;
        }

        inside_radius_timer = false;

        if (! pressureRead){ // if the keys were placed into the dish, switch to sleep
          digitalWrite(WAITING_DEBUG, LOW);
          inside_radius = false;
          systemState = SLEEP;
          break;
        }

        if (distance > 0 && distance < DETECTION_RADIUS){ // still in the vicinity after 5 seconds, switch to alarm state
          digitalWrite(WAITING_DEBUG, LOW);
          inside_radius = false;
          systemState = ALARM;
          break;
        }
      }
      break;
    case ALARM:
      analogWrite(ALARM_DEBUG, 245); // Set the pin high
      // delay(30); // Wait for half the period (25ms for a 400Hz square wave)
      // analogWrite(ALARM_DEBUG, 1500); // Set the pin low
      // delay(30); // Wait for the other half of the period

      if(!pressureRead){
        digitalWrite(ALARM_DEBUG, LOW);
        systemState = SLEEP;
        break;
      }else if (buttonPressed){
        digitalWrite(ALARM_DEBUG, LOW);
        timestamp = millis();
        systemState = TIMER;
        break;
      }else if (distance > DETECTION_RADIUS){
        digitalWrite(ALARM_DEBUG, LOW);
        systemState = WAITING;
        break;
      }
      break;
  }
  // Add any other state-independent actions here
  // delay(100);
}
