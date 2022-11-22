#ifndef ARTBOX_H
#define ARTBOX_H
#include <Arduino.h>
bool isTroll = false; //this only needed for esp8266
String name = "artbox"; //this only needed for esp8266
String device = "artbox";
String color = "green";
int port = 3005;
int wsPort = 3006;
int previousRGB[] = { 30, 179, 0 };
#endif