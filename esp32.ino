#include <WiFi.h> // Include WIFi Library for ESP32
#include <WebServer.h> // Include WebSwever Library for ESP32
#include <ArduinoJson.h> // Include ArduinoJson Library
#include <Arduino.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include <WebSocketsServer.h>
#include "artbox.h"

const char* ssid = "TP-Link_16A6";  // Your SSID
const char* password = "80050551"; // Your Password

WebServer server(port);  // create instance for web server on port "80"
WebSocketsServer webSocket = WebSocketsServer(81);

#define RGB_RED A0
#define RGB_GREEN A1
#define RGB_BLUE A2
#define BUTTON_PIN 12
int buttonState = 0;
int previousButtonState = 0;
bool ledState = false;
int successRGB[] = { 225, 76, 255 };
int errorRGB[] = { 76, 255, 255 };
int neutralRGB[] = { 0, 0, 179 };

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  ledcAttachPin(RGB_RED, 1); // assign RGB led pins to channels
  ledcAttachPin(RGB_GREEN, 2);
  ledcAttachPin(RGB_BLUE, 3);
  // ledcAttachPin(STATUS_RGB_RED, 4); // assign RGB led pins to channels
  // ledcAttachPin(STATUS_RGB_GREEN, 5);
  // ledcAttachPin(STATUS_RGB_BLUE, 6);
  ledcSetup(1, 12000, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(2, 12000, 8);
  ledcSetup(3, 12000, 8);
  // ledcSetup(4, 12000, 8); // 12 kHz PWM, 8-bit resolution
  // ledcSetup(5, 12000, 8);
  // ledcSetup(6, 12000, 8);
  turnOffLed("artbox");
  //turnOffLed("status");
  // put your setup code here, to run once:
  Serial.begin(115200); // Init Serial for Debugging.
  WiFi.begin(ssid, password); // Connect to Wifi 
  while (WiFi.status() != WL_CONNECTED) { // Check if wifi is connected or not
    //turnOffLed("status");
    delay(500);
    Serial.print(".");
    //changeStatus(neutralRGB);
    //delay(500);
  }
  Serial.println();
  // Print the IP address in the serial monitor windows.
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // Initialize a web server on the default IP address. and send the webpage as a response.
  server.enableCORS(true);
  server.begin(); // init the server
  webSocket.begin();  // init the Websocketserver
  webSocket.onEvent(webSocketEvent);  // init the webSocketEvent function when a websocket event occurs 
  Serial.println("HTTP server started");

  server.on("/", []() {
    Serial.println("request made to root");
    server.send(200, "text/plain", "Hello from ESP32!");
  });

  server.on("/status", []() {
    StaticJsonDocument doc = getStatus();
    String status;
    serializeJson(doc, status);
    Serial.println(status);
    server.send(200, F("application/json"), status);
  });

  server.on("/off", []() {
    turnOffLed(false);
    ledState = false;
    const int capacity = JSON_OBJECT_SIZE(6);
      StaticJsonDocument<capacity> doc;
      doc["isOn"] = ledState;
      doc["green"] = previousRGB[1];
      doc["blue"] = previousRGB[2];
      doc["red"] = previousRGB[0];
      doc["color"] = color;
      
      String buf;
      serializeJson(doc, buf);
      server.send(200, F("application/json"), buf);
  });

  server.on("/rgb", []() {
    String data_string = server.arg("plain");
    StaticJsonDocument<400> jDoc;
    DeserializationError err = deserializeJson(jDoc, data_string);
    JsonObject object = jDoc.as<JsonObject>();
    String name = object["name"];
    color = name;
    int rgbRed = (int)object["red"];
    int rgbGreen = (int)object["green"];
    int rgbBlue = (int)object["blue"];
    ledcWrite(1, rgbRed); // write red component to channel 1, etc.
    ledcWrite(2, rgbGreen);   
    ledcWrite(3, rgbBlue); 
    previousRGB[0] = rgbRed;
    previousRGB[1] = rgbGreen;
    previousRGB[2] = rgbBlue;
    ledState = true;
    StaticJsonDocument doc = getStatus();
    String status;
    serializeJson(doc, status);
    Serial.println(status);
    server.send(200, F("application/json"), status);
  });
}

void loop() {
  server.handleClient();  // webserver methode that handles all Client
  webSocket.loop(); // websocket server methode that handles all Client
  buttonState = digitalRead(BUTTON_PIN);

  if(WiFi.status() == WL_CONNECTED) {
    // changeStatus(successRGB);
  } else {
    // changeStatus(errorRGB);
  }

  if(previousButtonState && !buttonState) {
    if(ledState) {
      turnOffLed(device);
      ledState = false;
    } else {
      ledState = true;
      if(color != "rainbow") {
        ledcWrite(1, previousRGB[0]);
        ledcWrite(2, previousRGB[1]);   
        ledcWrite(3, previousRGB[2]); 
      }
    }

    String status = getStatus();
    Serial.println(buf);
    StaticJsonDocument doc = getStatus();
    String status;
    serializeJson(doc, status);

    if(color == "rainbow") {
      doc["rainbow"] = ledState; //this might be different need some way to communicate
    }

    webSocket.broadcastTXT(status);
  }

  previousButtonState = buttonState;
  delay(100);
}

//String getStatus() {
StaticJsonDocument getStatus() {
    //todo turn on and off status 
    //turnOffLed("status");
    //delay(500);
    //Serial.print(".");
    //changeStatus(neutralRGB);
    //delay(500);
    const int capacity = JSON_OBJECT_SIZE(6);
    StaticJsonDocument<capacity> doc;
    doc["green"] = previousRGB[1];
    doc["blue"] = previousRGB[2];
    doc["red"] = previousRGB[0];
    doc["color"] = color;
    doc["isOn"] = ledState;
    return doc;
    // String buf;

    // serializeJson(doc, buf);
    // return buf;
}

void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED: // enum that read status this is used for debugging.
      Serial.print("WS Type ");
      Serial.print(type);
      Serial.println(": DISCONNECTED");
      break;
    case WStype_CONNECTED:  // Check if a WebSocket client is connected or not
      Serial.print("WS Type ");
      Serial.print(type);
      Serial.println(": CONNECTED");
//      if (digitalRead(22) == HIGH) {  //check if pin 22 is high or low
//        pin_status = "ON";
//        update_webpage(); // update the webpage accordingly
//      }
//      else {                          
//        pin_status = "OFF"; //check if pin 22 is high or low
//        update_webpage();// update the webpage accordingly
//      }
      break;
    case WStype_TEXT: // check responce from client
//      if (payload[0] == '1') { 
//        pin_status = "ON";
//        digitalWrite(22, HIGH);               
//      }
//      if (payload[0] == '0') {
//        pin_status = "OFF";
//        digitalWrite(22, LOW);             
//      }
      break;
  }
}

void turnOffLed(String device) {
  int redChanel = 4;
  int greenChanel = 5;
  int blueChanel = 6;
  int off[] = { 0, 0, 0 };

  if(device != "status") {
    redChanel = 1;
    greenChanel = 2;
    blueChanel = 3;
  }

  if(device == "artbox") {
    ledcWrite(1, 255); // write red component to channel 1, etc.
    ledcWrite(2, 255);   
    ledcWrite(3, 255); 
  }

    ledcWrite(redChanel, 255); // write red component to channel 1, etc.
    ledcWrite(greenChanel, 255);   
    ledcWrite(blueChanel, 255); 
}

void changeStatus(int rgb[]) {
    ledcWrite(4, rgb[0]); // write red component to channel 1, etc.
    ledcWrite(5, rgb[1]);   
    ledcWrite(6, rgb[2]); 
}