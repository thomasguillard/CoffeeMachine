/*
  Controller.h - Support Library to control a coffee machine.
*/
#include "RTClib.h"
#include "Arduino.h"
#include "ControlMode.h"
#include "WiFiLink.h"

#ifndef Controller_h
#define Controller_h

class Controller
{
public:
  Controller(uint8_t relayPin,
             WiFiLink wifiLink);
  void setup();
  void tick();
  char *showMode();
  bool isReady();
  bool isStarted();
  ControlMode currentMode();
  void setMode(ControlMode mode);
  void toggleIsReady();
  void setSchedule(uint8_t hours, uint8_t minutes, uint8_t keepWarmDuration);
  char *getSchedule();

private:
  ControlMode _mode = ControlMode::off;
  uint8_t _relayPin;
  bool _isStarted = false;
  bool _isReady = false;
  void manage();
  void stop();
  void start();
  void control(bool state);
  void setIsReady(bool isReady);
  bool shouldStart();
  bool shouldStop();
  WiFiLink _wiFiLink;
  RTC_DS3231 _rtc;
  DateTime _startDateTime;
  DateTime _endDateTime;
};

#endif