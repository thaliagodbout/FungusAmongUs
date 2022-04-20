/* 
 * Mushroom code for Physical Computing 2 Final Project
 * Created by Thalia Godbout, Anne Vu, Joshuah Shillingford, Joseph Lawlor
 * April 6, 2022
 */

#include "EspMQTTClient.h"

EspMQTTClient client(
  "XXXXX", //SSID . "the name of your wifi Network"
  "XXXXX", //pssword . "password of your network"
  "XXXXX.cloud.shiftr.io", // Location of Shiftr Instance
  "XXXXX", //Name of Shiftr Instance
  "XXXXX", //Token Secret
  /* CHANGE "Thalia" TO YOUR NAME */
  "ESP32-Thalia", //ClientID
   1883  // port
);

// Pins for touch sensors
const int touchPin1 = 32;
const int touchPin2 = 33;
const int touchPin3 = 27;

const int ONBOARD_LED = 2;

// Reading for each touch sensor on the mushroom
int touchValue1 = 0;
int touchValue2 = 0;
int touchValue3 = 0;

int touchPosition = 0; // Which sensor the hand is currently on (1, 2, or 3. If 0, no sensor is being touched)
int lastTouchPosition = 0; // Previous position to track state changes

// Threshold set based on baselines
int touchBaselines[] = {0, 0, 0};
int touchThresholds[] = {0, 0, 0};

void setup() {
  Serial.begin(9600);
  calibrateTouchSensor();
}

void calibrateTouchSensor() {
  // Calibrate touch readings for 5 seconds to set a baseline for each sensor
  pinMode(ONBOARD_LED, OUTPUT); // Calibration indicator
  digitalWrite(ONBOARD_LED, HIGH); // Start calibration

  while (millis() < 5000) {
    touchBaselines[0] = getFilteredReading(touchPin1);
    touchBaselines[1] = getFilteredReading(touchPin2);
    touchBaselines[2] = getFilteredReading(touchPin3);
  }

  // Set thresholds when value goes a bit below baselines
  for (int i = 0; i < 3; i++) {
    touchThresholds[i] = touchBaselines[i] - 5;
  }
  
  digitalWrite(ONBOARD_LED, LOW); // Finish calibration
}

void onConnectionEstablished() {
  Serial.println("Successfully connected to wifi...");

  /* Subscribe to shiftr (for debugging purposes, uncomment below and change to your name)
  client.subscribe("Seals/Thalia/touchPosition", onMessageReceived);
  */
}

void loop() {
  client.loop();
  delay(100);

  touchValue1 = getFilteredReading(touchPin1);
  touchValue2 = getFilteredReading(touchPin2);
  touchValue3 = getFilteredReading(touchPin3);

  Serial.print("touchValue1: ");
  Serial.print(touchValue1);
  Serial.print(" touchValue2: ");
  Serial.print(touchValue2);
  Serial.print(" touchValue3: ");
  Serial.println(touchValue3);

  // Update current position
  checkPosition();

  // Publish which sensor is being touched
  publishOnTouch();
}

int getFilteredReading(int touchPin) {
  // Take the highest value of 2 readings to filter out low spikes
  int touchReading1 = touchRead(touchPin);
  int touchReading2 = touchRead(touchPin);
  return max(touchReading1, touchReading2);
}

void checkPosition() {
  // Check which hand the sensor is on and update onTouch value
  if (touchValue1 < touchThresholds[0]) {
    touchPosition = 1;
  }
  else if (touchValue2 < touchThresholds[1]) {
    touchPosition = 2;
  }
  else if (touchValue3 < touchThresholds[2]) {
    touchPosition = 3;
  }
  else {
    touchPosition = 0;
  }
}

void publishOnTouch() {
  if (touchPosition != lastTouchPosition) {
    
    /* CHANGE "Thalia" TO YOUR NAME */
    client.publish("Seals/Thalia/touchPosition", String(touchPosition));
    
    lastTouchPosition = touchPosition;
  }
}

void onMessageReceived(const String& topic, const String& message) {
  /* Check that values are publishing to shiftr (for debugging purposes, uncomment below and change to your name)
  if (topic == "Seals/Thalia/touchPosition") {
    touchPosition = message.toInt();
    Serial.print("Successfully published touchPosition: ");
    Serial.println(touchPosition);
  } */
}
