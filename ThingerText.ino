// Display any text pushed through Thinger.io
// Uses Node MCU ESP8266
// MAX7219 - 4 or 8 units
// Connections for MAX7219 - 3.3V, GND, DIN - D7, CS - D4, CLK - D5

#include <ESP8266WiFi.h>
#include <ThingerESP8266.h>
#include <SPI.h>
#include "LedMatrix.h"
#include <ThingerWifi.h>


char *ssid = "<ENTER WIFI SSID>";
char *password = "<ENTER WIFI PASSPHRASE>";

const char* thingerUser = "<THINGER USER>";
const char* deviceCode = "<THINGER DEVICE SECRET>";
const char* deviceName = "<THINGER DEVICE NAME>";

ThingerWifi thing(thingerUser, deviceName, deviceCode);

String displayTextCache;

// Change this based on how many devices are connected.
#define NUMBER_OF_DEVICES 8
#define CS_PIN D4
LedMatrix ledMatrix = LedMatrix(NUMBER_OF_DEVICES, CS_PIN);

void setup() {

  Serial.begin(115200);

  connectToWiFi();
  initializeLEDScreen();
  initializeThingerAPI();
}

// Connect to Thinger.io and initialize end points.
// This is the device endpoint that needs to be hit from a 3rd party service and
// text is sent as 'text' value in JSON.
// { "in" : {"text" : "Text Sent via Thinger"}}
void initializeThingerAPI() {
  thing["display_text"] << [](pson & in) {
    if (!in.is_empty()) {
      String toDisplayText = in["text"];
      writeTextToDisplay(toDisplayText);
      Serial.println("Received Display Text - " + toDisplayText);
    }
  };

  Serial.println("Thinger Initialized");
}

// If you're getting compilation errors because of squix78's library
// use this instead - https://github.com/adityarao/MAX7219LedMatrix
void initializeLEDScreen() {
  ledMatrix.init();
  ledMatrix.setIntensity(10);
  ledMatrix.setDeviceOrientation(VERTICAL);
  ledMatrix.scrollTextLeft();
  SPI.setClockDivider(SPI_CLOCK_DIV64);
  ledMatrix.setText("Showing Default text - Change this");
  Serial.println("LED Initialization Complete");

}

void loop() {
  unsigned long startTime = millis();
  thing.handle();
  unsigned long endTime = (millis() - startTime)/1000;
  if(endTime > 5) {
    Serial.println(endTime);
  }

  writeTextToDisplay("");
}

void writeTextToDisplay(String displayText) {

  if (!displayText.length() == 0 ) {
    displayTextCache = displayText;
  }

  ledMatrix.clear();
  ledMatrix.scrollTextLeft();
  ledMatrix.drawText();
  ledMatrix.commit();

  ledMatrix.setNextText(displayTextCache);
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wifi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("-");
    delay(500);
  }
  Serial.println("");
  Serial.println("IP Address : ");
  Serial.println(WiFi.localIP());

}
