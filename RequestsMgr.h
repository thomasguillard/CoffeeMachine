/*
  RequestsMgr.h - Support Library to manage http requests.
*/

#include "Arduino.h"
#include "ControlMode.h"
#include "Controller.h"
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef RequestsMgr_h
#define RequestsMgr_h

class RequestsMgr
{
public:
    RequestsMgr(uint8_t serverPort, 
                Controller *controller);
    void setup();
    void tick();

private:
    ESP8266WebServer _webServer;
    Controller *_controller;
    void handleRoot();
    void handleInfo();
    void handleSetMode();
    void handleGetSchedule();
    void handleSetSchedule();
    void handleNotFound();
    const char* CONTENT_TYPE = "text/plain";
    const int OK = 200;
    const int BAD_REQUEST = 400;
    const int FORBIDDEN = 403;
    const int NOT_FOUND = 404;
    const int METHOD_NOT_ALLOWED = 405;
};

#endif