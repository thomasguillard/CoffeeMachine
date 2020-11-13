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
             int timeRefreshInterval);
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
  bool shouldStart();
  bool shouldStop();
  WiFiLink _wiFiLink;
  TimeKeeper _timeKeeper;
};

#endif