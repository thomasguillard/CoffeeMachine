/*
  WiFiLink.h - Support Library to connect to Wifi.
*/

#include <ESP8266WiFi.h>

#ifndef WiFiLink_h
#define WiFiLink_h

class WiFiLink
{
public:
    WiFiLink();
    WiFiLink(char *ssid, char *password);
    void setup();

private:
    char *_ssid;
    char *_password;
};

#endif