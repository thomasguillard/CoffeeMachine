/*
  Controller.h - Support Library to control a coffee machine.
*/
#include "ControlMode.h"
#include "WiFiLink.h"
#include "TimeKeeper.h"
#include <OneButton.h>

#ifndef Controller_h
#define Controller_h

class Controller
{
public:
  Controller(uint8_t relayPin,
             uint8_t isSchedulePin,
             uint8_t isReadyPin,
             uint8_t isOnPin,
             char *ssid,
             char *password,
             char *ntpServer,
             char *timeZone,
             uint16_t timeRefreshInterval,
             uint8_t targetTimeHours,
             uint8_t targetTimeMinutes,
             uint8_t keepWarmDuration);
  void setup();
  void tick();
  // TODO: Temporary
  String showMode();
  bool isReady();
  bool isStarted();

private:
  ControlMode _mode = ControlMode::off;
  uint8_t _relayPin;
  uint8_t _isSchedulePin;
  uint8_t _isReadyPin;
  uint8_t _isOnPin;
  bool _isStarted = false;
  bool _isReady = false;
  OneButton _btn;
  static void btnCallback(void *ptr);
  void evaluateMode();
  void manage();
  void stop();
  void start();
  void control(bool state);
  uint8_t _keepWarmDuration;
  uint8_t _targetTimeHours;
  uint8_t _targetTimeMinutes;
  bool shouldStart();
  bool shouldStop();
  WiFiLink _wiFiLink;
  TimeKeeper _timeKeeper;
};

#endif