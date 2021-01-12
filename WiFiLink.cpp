/*
  WiFiLink.cpp - Support Library to connect to Wifi.
*/

#include "Arduino.h"
#include "WiFiLink.h"
#include <ESP8266WiFi.h>

// Constructors
WiFiLink::WiFiLink() {}
WiFiLink::WiFiLink(char *ssid, char *password)
{
    _ssid = ssid;
    _password = password;
}

// Public
void WiFiLink::setup()
{
    WiFi.begin(_ssid, _password);
    int counter = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(200);
        if (++counter > 100)
            ESP.restart();
    }
    Serial.println("IP address:");
    Serial.println(WiFi.localIP());
}

// Private