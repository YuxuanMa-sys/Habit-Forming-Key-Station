#include <SPI.h>

#define BUTTON_PIN 2           // Digital input pin for the button
#define PRESSURE_SENSOR_PIN A0 // Analog input pin that the pressure sensor is attached to
// SPI Transceiver CS (Slave Select) pin
#define TRANSCEIVER_CS_PIN 10
#define SPEAKER_PIN 4

// Threshold for the force resistor's reading to switch from SLEEP to TIMER state
const int FORCE_THRESHOLD = 500; // Example value; adjust based on your calibration
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

// Function that needs to be written to work with the transceiver
  bool checkForResponse() {
    return true;
  } 

void setup() {
  // Initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  
  // Initialize SPI:
  SPI.begin();
  
  // Set the transceiver CS pin as an output:
  pinMode(TRANSCEIVER_CS_PIN, OUTPUT);

  // Set the CS pin high to start with the transceiver not selected
  digitalWrite(TRANSCEIVER_CS_PIN, HIGH);
  
  // Configure the button pin as an input with an internal pull-up resistor
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(SPEAKER_PIN, OUTPUT);
}

void loop() {

  unsigned long current_time = millis();
  int pressureValue = analogRead(PRESSURE_SENSOR_PIN);

  // Read the button state
  bool buttonPressed = digitalRead(BUTTON_PIN) == LOW;

  switch (systemState) {
    case SLEEP:     
      // Check if the pressure is below the threshold
      if (pressureValue < FORCE_THRESHOLD) {
        time_keys_left = millis();
        systemState = TIMER; // Transition to the TIMER state
      }
      break;
    case TIMER:
      // Perform actions for STATE_2
      if(current_time - time_keys_left >= 5000){
        systemState = WAITING;
      }else if(pressureValue > FORCE_THRESHOLD){
        systemState = SLEEP;
      }
      break;
    case WAITING:
      if (pressureValue > FORCE_THRESHOLD){
        systemState = SLEEP;
      }else if (current_time - transceiver_timer >= 5000) {
        // Save the last time you broadcasted
        transceiver_timer = current_time;

        // Select the transceiver:
        digitalWrite(TRANSCEIVER_CS_PIN, LOW);
        
        // Send the "ping" signal to other devices
        SPI.transfer(0x01); // Example byte to send
        
        // Deselect the transceiver
        digitalWrite(TRANSCEIVER_CS_PIN, HIGH);

        // Check for a response
        if (checkForResponse()) {
          systemState = ALARM; // Transition to the ALARM state if a response is detected
        }

      }
      break;
    case ALARM:
      digitalWrite(SPEAKER_PIN, HIGH); // Set the pin high
      delay(25); // Wait for half the period (25ms for a 400Hz square wave)
      digitalWrite(SPEAKER_PIN, LOW); // Set the pin low
      delay(25); // Wait for the other half of the period

      if(buttonPressed || pressureValue > FORCE_THRESHOLD){
        systemState = SLEEP;
      }

      break;
  }
  // Add any other state-independent actions here

}
