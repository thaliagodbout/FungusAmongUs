/* 
 * MYCELIUM code for Physical Computing 2 Final Project
 * Created by Thalia Godbout, Anne Vu, Joshuah Shillingford, Joseph Lawlor
 * April 6, 2022
 */

#include <Adafruit_NeoPixel.h>
#include "EspMQTTClient.h"

#define PIN 23
#define NUMPIXELS 100

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

EspMQTTClient client(
  "XXXXX", //SSID . "the name of your wifi Network"
  "XXXXX", //pssword . "password of your network"
  "XXXXX.cloud.shiftr.io", // Location of Shiftr Instance
  "XXXXX", //Name of Shiftr Instance
  "XXXXX", //Token Secret
  "Mycelium", //ClientID
   1883  // port
);

// Touch position of each mushroom
int anneTouchPosition;
int joshTouchPosition;
int joeTouchPosition;
int thaliaTouchPosition;

// Array representing four different mushrooms
// Each value represents which sensor each person is touching on their mushroom
// e.g. {2, 0, 1, 0} means mushroom 1 is touching sensor 2, mushroom 3 is touching sensor 1
// {Anne, Josh, Joe, Thalia}
int onTouch[] = {0, 0, 0, 0};

// Start and end pixels of each section on the LED strip
uint16_t pixelSection1_start = 0;
uint16_t pixelSection1_end = 32;
uint16_t pixelSection2_start = 33;
uint16_t pixelSection2_end = 65;
uint16_t pixelSection3_start = 66;
uint16_t pixelSection3_end = 99;

unsigned long lastMillis = 0;
int crashCounter = 0; // Crash counter to show how long ESP is connected to wifi

void setup() {
  Serial.begin(9600);
  pixels.begin(); // Initialize NeoPixel strip object
}

void onConnectionEstablished() {
  client.subscribe("Seals/Anne/touchPosition", onMessageReceived);
  client.subscribe("Seals/Joe/touchPosition", onMessageReceived);
  client.subscribe("Seals/Thalia/touchPosition", onMessageReceived);
  client.subscribe("Seals/Josh/touchPosition", onMessageReceived);
}

void loop() {
  client.loop();
  delay(10);
  
  if (millis() - lastMillis > 10000) {
    lastMillis = millis();
    client.publish("Seals/Mycelium/crashCounter", String(crashCounter)); // publishing a count
    crashCounter++;
  }

  // Store colours for each section along the LED strip
  uint32_t section1Colour = pixels.Color(0, 0, 0);
  uint32_t section2Colour = pixels.Color(0, 0, 0);
  uint32_t section3Colour = pixels.Color(0, 0, 0);

  // Add colours for each section depending on who's touching which sensors
  for (int i = 0; i < 4; i++) {
    if (onTouch[i] == 1) {
      section1Colour = addColours(section1Colour, i); // Add colour of mushroom i to section 1 colour
    } else if (onTouch[i] == 2) {
      section2Colour = addColours(section2Colour, i); // Add colour of mushroom i to section 2 colour
    } else if (onTouch[i] == 3) {
      section3Colour = addColours(section3Colour, i); // Add colour of mushroom i to section 3 colour
    }
  }
 
  // Turn 3 sections on using calculated colours
  lightsOnPlease(pixelSection1_start, pixelSection1_end, section1Colour);
  lightsOnPlease(pixelSection2_start, pixelSection2_end, section2Colour);
  lightsOnPlease(pixelSection3_start, pixelSection3_end, section3Colour);
}

// Lines commented out are currently causing issues
void lightsOnPlease(uint16_t start_index, uint16_t end_index, uint32_t color) {
  for (uint16_t i = start_index; i <= end_index; i++) {
    pixels.setPixelColor(i, color); // Set LED strip's colours
  }
  pixels.show(); // Turn LED strip on
}

// Given an original colour and a Mushroom id, add the colours, and return a packed RGB Color
uint32_t addColours(uint32_t oldColour, int id) {
  // Unpack the RGB values of old colour into separate components
  int newR = Red(oldColour);
  int newG = Green(oldColour);
  int newB = Blue(oldColour);
  
  if (id == 0) { // Add green values (G) for mushroom 0
    newG += 70;
  } else if (id == 1) { // Add blue value (B) for mushroom 1
    newB += 70;
  } else if (id == 2) { // Add yellow values (R and G) for mushroom 2
    newR += 70;
    newG += 70;
  } else if (id == 3) { // Add red vaue (R) for mushroom 3
    newR += 70;
  }

  return (pixels.Color(newR, newG, newB));
}

// Returns the Red component of a 32-bit color
// https://learn.adafruit.com/multi-tasking-the-arduino-part-3/utility-functions
uint8_t Red(uint32_t color) {
  return (color >> 16) & 0xFF;
}

// Returns the Green component of a 32-bit color
uint8_t Green(uint32_t color) {
  return (color >> 8) & 0xFF;
}

// Returns the Blue component of a 32-bit color
uint8_t Blue(uint32_t color) {
  return color & 0xFF;
}

void onMessageReceived(const String& topic, const String& message) {
  if (topic == "Seals/Anne/touchPosition") {
    anneTouchPosition = message.toInt();
    onTouch[0] = anneTouchPosition;
    Serial.print("anneTouchPosition: ");
    Serial.println(anneTouchPosition);
  }

  if (topic == "Seals/Josh/touchPosition") {
    joshTouchPosition = message.toInt();
    onTouch[1] = joshTouchPosition;
    Serial.print("joshTouchPosition: ");
    Serial.println(joshTouchPosition);
  }

  if (topic == "Seals/Joe/touchPosition") {
    joeTouchPosition = message.toInt();
    onTouch[2] = joeTouchPosition;
    Serial.print("joeTouchPosition: ");
    Serial.println(joeTouchPosition);
  }

  if (topic == "Seals/Thalia/touchPosition") {
    thaliaTouchPosition = message.toInt();
    onTouch[3] = thaliaTouchPosition;
    Serial.print("thaliaTouchPosition: ");
    Serial.println(thaliaTouchPosition);
  }
}
