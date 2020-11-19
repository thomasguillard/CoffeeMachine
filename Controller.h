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
             uint8_t isOffModeBtnPin,
             uint8_t isOffModeLedPin,
             uint8_t isScheduleModeBtnPin,
             uint8_t isScheduleModeLedPin,
             uint8_t isOnModeBtnPin,
             uint8_t isOnModeLedPin,
             uint8_t isReadyBtnPin,
             uint8_t isReadyLedPin,
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
  uint8_t _isOffModeBtnPin;
  uint8_t _isOffModeLedPin;
  uint8_t _isScheduleModeBtnPin;
  uint8_t _isScheduleModeLedPin;
  uint8_t _isOnModeBtnPin;
  uint8_t _isOnModeLedPin;
  uint8_t _isReadyBtnPin;
  uint8_t _isReadyLedPin;
  bool _isStarted = false;
  bool _isReady = false;
  OneButton _isOffModeBtn;
  OneButton _isScheduleModeBtn;
  OneButton _isOnModeBtn;
  OneButton _isReadyBtn;
  static void isOffModeBtnCallback(void *ptr);
  static void isScheduleModeBtnCallback(void *ptr);
  static void isOnModeBtnCallback(void *ptr);
  void setMode(ControlMode mode);
  static void isReadyBtnCallback(void *ptr);
  void setIsReady(bool isReady);
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