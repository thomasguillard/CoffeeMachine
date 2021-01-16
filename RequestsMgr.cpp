/*
  RequestsMgr.cpp - Support Library to manage http requests.
*/

#include "RequestsMgr.h"
#include <ESP8266WebServer.h>
#include <stdio.h>

// Constructors
RequestsMgr::RequestsMgr(uint8_t serverPort,
                         Controller *controller)
    : _webServer(serverPort),
      _controller(controller)
{
}

// Public
void RequestsMgr::setup()
{
  _webServer.on("/", std::bind(&RequestsMgr::handleRoot, this));
  _webServer.on("/info", std::bind(&RequestsMgr::handleInfo, this));
  _webServer.on("/setMode", std::bind(&RequestsMgr::handleSetMode, this));
  _webServer.on("/getSchedule", std::bind(&RequestsMgr::handleGetSchedule, this));
  _webServer.on("/setSchedule", std::bind(&RequestsMgr::handleSetSchedule, this));
  _webServer.onNotFound(std::bind(&RequestsMgr::handleNotFound, this));
  _webServer.begin();
}

void RequestsMgr::tick()
{
  _webServer.handleClient();
}

// Private
void RequestsMgr::handleRoot()
{
  _webServer.send(this->OK,
                  this->CONTENT_TYPE,
                  "Coffee Machine says hi!");
}

void RequestsMgr::handleInfo()
{
  char res[100];
  snprintf(res,
           sizeof(res),
           "Info:\n-Mode: %s\n-IsReady: %s",
           _controller->showMode(),
           _controller->isReady() ? "Yes" : "No");

  _webServer.send(this->OK,
                  this->CONTENT_TYPE,
                  res);
}

void RequestsMgr::handleSetMode()
{
  if (_webServer.method() != HTTP_POST)
  {
    _webServer.send(this->METHOD_NOT_ALLOWED,
                    this->CONTENT_TYPE,
                    "Expecting 'Post'.");
    return;
  }

  if (!_webServer.hasArg("controlMode"))
  {
    _webServer.send(this->BAD_REQUEST,
                    this->CONTENT_TYPE,
                    "Expecting a 'controlMode' argument.");
    return;
  }

  const String controlModeArgValue = _webServer.arg("controlMode");
  char requestedMode[200];
  controlModeArgValue.toCharArray(requestedMode, sizeof(requestedMode));

  ControlMode nextMode;
  if (controlModeArgValue == "Off")
  {
    nextMode = ControlMode::off;
  }
  else if (controlModeArgValue == "Schedule")
  {
    nextMode = ControlMode::schedule_check;
  }
  else if (controlModeArgValue == "On")
  {
    if (!_controller->isReady())
    {
      _webServer.send(this->FORBIDDEN,
                      this->CONTENT_TYPE,
                      "Machine cannot be started if not physically flagged as ready.");
      return;
    }
    nextMode = ControlMode::on;
  }
  else
  {
    char res[100];
    snprintf(res,
             sizeof(res),
             "'%s' is not a valid 'controlMode' argument value.",
             requestedMode);

    _webServer.send(this->BAD_REQUEST,
                    this->CONTENT_TYPE,
                    res);
    return;
  }

  _controller->setMode(nextMode);

  char res[100];
  snprintf(res,
           sizeof(res),
           "ControlMode '%s' applied.",
           requestedMode);

  _webServer.send(this->OK,
                  this->CONTENT_TYPE,
                  res);
}

void RequestsMgr::handleGetSchedule()
{
  _webServer.send(this->OK,
                  this->CONTENT_TYPE,
                  _controller->getSchedule());
}

void RequestsMgr::handleSetSchedule()
{
  if (_webServer.method() != HTTP_POST)
  {
    _webServer.send(this->METHOD_NOT_ALLOWED,
                    this->CONTENT_TYPE,
                    "Expecting 'Post'.");
    return;
  }

  if (!_webServer.hasArg("hour"))
  {
    _webServer.send(this->BAD_REQUEST,
                    this->CONTENT_TYPE,
                    "Expecting a 'hour' argument.");
    return;
  }

  if (!_webServer.hasArg("minute"))
  {
    _webServer.send(this->BAD_REQUEST,
                    this->CONTENT_TYPE,
                    "Expecting a 'minute' argument.");
    return;
  }

  if (!_webServer.hasArg("keepWarmDuration"))
  {
    _webServer.send(this->BAD_REQUEST,
                    this->CONTENT_TYPE,
                    "Expecting a 'keepWarmDuration' argument.");
    return;
  }

  const String hourArgValue = _webServer.arg("hour");
  const String minuteArgValue = _webServer.arg("minute");
  const String keepWarmDurationArgValue = _webServer.arg("keepWarmDuration");
  
  const uint8_t hour = (uint8_t)hourArgValue.toInt();
  const uint8_t minute = (uint8_t)minuteArgValue.toInt();
  const uint8_t keepWarmDuration = (uint8_t)keepWarmDurationArgValue.toInt();

  _controller->setSchedule(hour, minute, keepWarmDuration);

  _webServer.send(this->OK,
                  this->CONTENT_TYPE,
                  "New schedule applied");
}

void RequestsMgr::handleNotFound()
{
  _webServer.send(this->NOT_FOUND,
                  this->CONTENT_TYPE,
                  "Woohaa horsy, coffee machine has no idea what you are on about!");
}