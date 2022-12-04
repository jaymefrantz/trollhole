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
WebSocketsServer webSocket = WebSocketsServer(wsPort);

#define RGB_RED A0
#define RGB_GREEN A1
#define RGB_BLUE A5
 #define STATUS_RGB_RED 27
 #define STATUS_RGB_GREEN 33
 #define STATUS_RGB_BLUE 15
#define BUTTON_PIN 12
int buttonState = 0;
int previousButtonState = 0;
bool ledState = false;
int successRGB[] = { 0, 179, 0 };
int errorRGB[] = { 179, 0, 0 };
int neutralRGB[] = { 0, 0, 179 };
int wsRGB[] = { 179, 18, 0 };
bool changingStatus = false;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  ledcAttachPin(RGB_RED, 1); // assign RGB led pins to channels
  ledcAttachPin(RGB_GREEN, 2);
  ledcAttachPin(RGB_BLUE, 3);
  ledcAttachPin(STATUS_RGB_RED, 4); // assign RGB led pins to channels
  ledcAttachPin(STATUS_RGB_GREEN, 5);
  ledcAttachPin(STATUS_RGB_BLUE, 6);
  ledcSetup(1, 12000, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(2, 12000, 8);
  ledcSetup(3, 12000, 8);
  ledcSetup(4, 12000, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(5, 12000, 8);
  ledcSetup(6, 12000, 8);
  turnOffLed(device);
  turnOffLed("status");
  // put your setup code here, to run once:
  Serial.begin(115200); // Init Serial for Debugging.
  WiFi.begin(ssid, password); // Connect to Wifi 
  while (WiFi.status() != WL_CONNECTED) { // Check if wifi is connected or not
  turnOffLed("status");
    delay(500);
    Serial.print(".");
    changeStatus(successRGB);
    delay(500);
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
    String status = getStatus(false);
    Serial.println(status);
    server.send(200, F("application/json"), status);
    showStatusChange(neutralRGB, 3, 100);
  });

  server.on("/rgb", [](){
    String data_string = server.arg("plain");
    StaticJsonDocument<400> jDoc;
    DeserializationError err = deserializeJson(jDoc, data_string);
    JsonObject object = jDoc.as<JsonObject>();
    String name = object["name"];
    if(name == "null") {
      server.send(200, F("application/json"), "who is calling this??");
      return; //same route is called twice when using in browser, not postman, yet network only has one call
    }

    color = name;
    int rgbRed = (int)object["red"];
    int rgbGreen = (int)object["green"];
    int rgbBlue = (int)object["blue"];
    bool isOff = (rgbRed == 255 && rgbGreen == 255 && rgbBlue == 255 && isInverted) || (rgbRed == 0 && rgbGreen == 0 && rgbBlue == 0 && !isInverted);
    Serial.println(name);
    Serial.print(rgbRed);
    Serial.print(", ");
    Serial.print(rgbGreen);
    Serial.print(", ");
    Serial.print(rgbBlue);
    Serial.println(" ");
    if(isOff) {
      turnOffLed(device);
      ledState = false;
    } else {
      previousRGB[0] = rgbRed;
      previousRGB[1] = rgbGreen;
      previousRGB[2] = rgbBlue;
      ledState = true;
    }

    String status = getStatus(false);
    //Serial.println(status);
    //webSocket.broadcastTXT(status);
    ledcWrite(1, rgbRed); // write red component to channel 1, etc.
    ledcWrite(2, rgbGreen);   
    ledcWrite(3, rgbBlue); 

    server.send(200, F("application/json"), status);
    if(color != "rainbow") {
      showStatusChange(neutralRGB, 3, 100);    
    }
  });

   server.on("/status/rgb", []() {
     String data_string = server.arg("plain");
     StaticJsonDocument<400> jDoc;
     DeserializationError err = deserializeJson(jDoc, data_string);
     JsonObject object = jDoc.as<JsonObject>();
     int rgbRed = (int)object["red"];
     int rgbGreen = (int)object["green"];
     int rgbBlue = (int)object["blue"];
     ledcWrite(4, 255 - rgbRed); // write red component to channel 1, etc.
     ledcWrite(5, 255 - rgbGreen);   
     ledcWrite(6, 255 - rgbBlue);
     Serial.print(rgbRed);
      Serial.print(", ");
      Serial.print(rgbGreen);
      Serial.print(", ");
      Serial.print(rgbBlue);
      Serial.println(" ");
     String status = getStatus(false);
     server.send(200, F("application/json"), status);
   });
}

void loop() {
  server.handleClient();  // webserver methode that handles all Client
  webSocket.loop(); // websocket server methode that handles all Client
  buttonState = digitalRead(BUTTON_PIN);

  // if(!changingStatus) {
  //   if(WiFi.status() == WL_CONNECTED) {
  //     changeStatus(successRGB);
  //   } else {
  //     changeStatus(errorRGB);
  //   }
  // }

  if(previousButtonState && !buttonState) {
    Serial.println("i know im pressing button?");
    if(ledState) {
      Serial.println("im trying to turn off?");
      turnOffLed(device);
      ledState = false;
    } else {
      Serial.println("i am trying to turn back on?");
      ledState = true;
      if(color != "rainbow") {
        ledcWrite(1, previousRGB[0]);
        ledcWrite(2, previousRGB[1]);   
        ledcWrite(3, previousRGB[2]); 
      }
    }

    String status = getStatus(false);
    webSocket.broadcastTXT(status);
    showStatusChange(neutralRGB, 3, 100);
  }

  previousButtonState = buttonState;
  //Serial.println(changingStatus);
  delay(100);
}

//void showStatusChange() {
void showStatusChange(int rgb[], int count, int delayMs) {
  //Serial.println(rgb);
  changingStatus = true;

  for (int i = 0; i <= count; i++) {
    changeStatus(rgb);
    delay(delayMs);
    turnOffLed("status");
    delay(delayMs);
  }

  changingStatus = false;
}

String getStatus(bool initial) {
    //todo turn on and off status 
    turnOffLed("status");
    const int capacity = JSON_OBJECT_SIZE(8);
    StaticJsonDocument<capacity> doc;
    doc["green"] = previousRGB[1];
    doc["blue"] = previousRGB[2];
    doc["red"] = previousRGB[0];
    doc["color"] = color;
    doc["isOn"] = ledState;
    doc["device"] = device;
    doc["initial"] = initial;
    String status;
    serializeJson(doc, status);
    return status;
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
      String status = getStatus(true);
      webSocket.broadcastTXT(status);
      showStatusChange(wsRGB, 3, 100);
      break;
  }
}

void turnOffLed(String device) {
   int redChanel = 1;
   int greenChanel = 2;
   int blueChanel = 3;
   int off[] = { 0, 0, 0 };

   if(device == "status") {
     redChanel = 4;
     greenChanel = 5;
     blueChanel = 6;
   }

   if(isInverted || device == "status") {
     off[0] = 255;
     off[1] = 255;
     off[2] = 255;
   }

  ledcWrite(redChanel, off[0]);
  ledcWrite(greenChanel, off[1]);   
  ledcWrite(blueChanel, off[2]);  
}

void changeStatus(int rgb[]) {
   ledcWrite(4, 255 - rgb[0]);
   ledcWrite(5, 255 - rgb[1]);   
   ledcWrite(6, 255 - rgb[2]); 
}