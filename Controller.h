/*
  Controller.h - Support Library to control a coffee machine.
*/
#include "ControlMode.h"
#include "WiFiLink.h"
#include "TimeKeeper.h"

#ifndef Controller_h
#define Controller_h

class Controller
{
public:
  Controller(uint8_t relayPin,
             WiFiLink wifiLink,
             TimeKeeper timeKeeper,
             uint8_t targetTimeHours,
             uint8_t targetTimeMinutes,
             uint8_t keepWarmDuration);
  void setup();
  void tick();
  char *showMode();
  bool isReady();
  bool isStarted();
  ControlMode currentMode();
  void setMode(ControlMode mode);
  void toggleIsReady();

private:
  ControlMode _mode = ControlMode::off;
  uint8_t _relayPin;
  bool _isStarted = false;
  bool _isReady = false;
  void manage();
  void stop();
  void start();
  void control(bool state);
  uint8_t _keepWarmDuration;
  uint8_t _targetTimeHours;
  uint8_t _targetTimeMinutes;
  void setIsReady(bool isReady);
  bool shouldStart();
  bool shouldStop();
  WiFiLink _wiFiLink;
  TimeKeeper _timeKeeper;
};

#endif