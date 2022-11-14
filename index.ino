#include <Arduino.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h> // Include ArduinoJson Library
#include "artbox.h"

const char *ssid = "TP-Link_16A6"; // Your SSID
const char *password = "80050551"; // Your Password
ESP8266WiFiMulti WiFiMulti;
ESP8266WebServer server(port);
#define RGB_RED D0
#define RGB_GREEN D1
#define RGB_BLUE D2

#define STATUS_RGB_RED D6
#define STATUS_RGB_GREEN D7
#define STATUS_RGB_BLUE D8

#define BUTTON_PIN D5
int buttonState = 0;
int previousButtonState = 0;
bool ledState = false;
int previousRGB[] = { 255, 0, 255 };

void turnStatusLedRed() {
   analogWrite(STATUS_RGB_RED, 76);
   analogWrite(STATUS_RGB_GREEN, 255);
   analogWrite(STATUS_RGB_BLUE, 255);
}

 void turnStatusLedGreen() {
     analogWrite(STATUS_RGB_RED, 225);
     analogWrite(STATUS_RGB_GREEN, 76);
     analogWrite(STATUS_RGB_BLUE, 255);
 }

void turnOffLed(bool isStatus) {
  int redPin = RGB_RED;
  int greenPin = RGB_GREEN;
  int bluePin = RGB_BLUE;

  if(isStatus) {
    redPin = STATUS_RGB_RED;
    greenPin = STATUS_RGB_GREEN;
    bluePin = STATUS_RGB_BLUE;
  } 

  if(isTroll || isStatus) {
    analogWrite(redPin, 255);
    analogWrite(greenPin, 255);
    analogWrite(bluePin, 255);
  } else {
    analogWrite(redPin, 0);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);
  }
}


void setup() {
  pinMode(RGB_RED, OUTPUT);
  pinMode(RGB_GREEN, OUTPUT);
  pinMode(RGB_BLUE, OUTPUT);
  pinMode(STATUS_RGB_RED, OUTPUT);
  pinMode(STATUS_RGB_GREEN, OUTPUT);
  pinMode(STATUS_RGB_BLUE, OUTPUT);
  pinMode(BUTTON_PIN,INPUT_PULLUP);
  turnOffLed(true); //turning off status
  turnOffLed(false); //turning off led
  // put your setup code here, to run once:
  Serial.begin(115200); // Init Serial for Debugging.

  WiFiMulti.addAP(ssid, password);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    turnOffLed(true);
    delay(500);
    Serial.print(".");
    turnStatusLedGreen();
    delay(500);
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.enableCORS(true);
  server.begin();
  Serial.println("HTTP server started");

  server.on("/", []() {
    server.send(200, "text/plain", "Hello from ESP8266!");
  });

server.on("/rgb", []() {
    String data_string = server.arg("plain");
    StaticJsonDocument<400> jDoc;
    DeserializationError err = deserializeJson(jDoc, data_string);
    JsonObject object = jDoc.as<JsonObject>();
    int rgbRed = (int)object["red"];
    int rgbGreen = (int)object["green"];
    int rgbBlue = (int)object["blue"];
    analogWrite(RGB_RED, rgbRed);
    analogWrite(RGB_GREEN, rgbGreen);
    analogWrite(RGB_BLUE, rgbBlue);
    previousRGB[0] = rgbRed;
    previousRGB[1] = rgbGreen;
    previousRGB[2] = rgbBlue;
    ledState = true;
    const int capacity = JSON_OBJECT_SIZE(3);
    StaticJsonDocument<capacity> doc;
    doc["green"] = rgbGreen;
    doc["blue"] = rgbBlue;
    doc["red"] = rgbRed;
    String buf;

    serializeJson(doc, buf);
    
    server.send(200, F("application/json"), buf);
    Serial.println("I have heard the request");
  });
}

void loop() {
  //Serial.println("I am in loop");
  //delay(1000);
  server.handleClient();
  if(WiFi.status() == WL_CONNECTED) {
    turnStatusLedGreen();
  } else {
    turnStatusLedRed();
  }

  buttonState = digitalRead(BUTTON_PIN);
  if(previousButtonState && !buttonState) {
    //see if it's on or off then do stuff...
    Serial.print("ledState is:");
    Serial.println(ledState);

    if(ledState) {
      turnOffLed(false);
      ledState = false;
    } else {
      ledState = true;
      analogWrite(RGB_RED, previousRGB[0]);
      analogWrite(RGB_GREEN, previousRGB[1]);
      analogWrite(RGB_BLUE, previousRGB[2]);
    }
  }

  previousButtonState = buttonState;
  delay(100);
}

