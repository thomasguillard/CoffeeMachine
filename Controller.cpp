/*
  Controller.cpp - Support Library to control a coffee machine.
*/

#include "Arduino.h"
#include "Controller.h"
#include "ControlMode.h"
#include <OneButton.h>

// Constructor
Controller::Controller(uint8_t relayPin,
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
                       uint8_t keepWarmDuration)
{
    _relayPin = relayPin;
    _isOffModeBtnPin = isOffModeBtnPin;
    _isOffModeLedPin = isOffModeLedPin;
    _isScheduleModeBtnPin = isScheduleModeBtnPin;
    _isScheduleModeLedPin = isScheduleModeLedPin;
    _isOnModeBtnPin = isOnModeBtnPin;
    _isOnModeLedPin = isOnModeLedPin;
    _isReadyBtnPin = isReadyBtnPin;
    _isReadyLedPin = isReadyLedPin;
    _wiFiLink = WiFiLink(ssid, password);
    _timeKeeper = TimeKeeper(ntpServer, timeZone, timeRefreshInterval);
    _targetTimeHours = targetTimeHours;
    _targetTimeMinutes = targetTimeMinutes;
    _keepWarmDuration = keepWarmDuration;
}

// Public
void Controller::setup()
{
    pinMode(_relayPin, OUTPUT);

    _isOffModeBtn = OneButton(_isOffModeBtnPin, true, true);
    _isScheduleModeBtn = OneButton(_isScheduleModeBtnPin, true, true);
    _isOnModeBtn = OneButton(_isOnModeBtnPin, true, true);
    _isReadyBtn = OneButton(_isReadyBtnPin, true, true);

    _isOffModeBtn.attachClick(isOffModeBtnCallback, this);
    _isScheduleModeBtn.attachClick(isScheduleModeBtnCallback, this);
    _isOnModeBtn.attachClick(isOnModeBtnCallback, this);
    _isReadyBtn.attachClick(isReadyBtnCallback, this);

    pinMode(_isOffModeLedPin, OUTPUT);
    pinMode(_isScheduleModeLedPin, OUTPUT);
    pinMode(_isOnModeLedPin, OUTPUT);
    pinMode(_isReadyLedPin, OUTPUT);

    stop();
    setMode(ControlMode::off);

    _wiFiLink.setup();
    _timeKeeper.setup();
}

void Controller::tick()
{
    _isOffModeBtn.tick();
    _isScheduleModeBtn.tick();
    _isOnModeBtn.tick();
    _isReadyBtn.tick();
    manage();
}

String Controller::showMode()
{
    switch (_mode)
    {
    case ControlMode::error:
        return "Error";
        break;

    case ControlMode::off:
        return "Off";
        break;

    case ControlMode::schedule_check:
        return "Schedule check";
        break;

    case ControlMode::schedule_fulfill:
        return "Schedule fulfill";
        break;

    case ControlMode::on:
        return "On";
        break;

    default:
        return "Unknown";
        break;
    }
}

bool Controller::isReady()
{
    return _isReady;
}

bool Controller::isStarted()
{
    return _isStarted;
}

// Private
void Controller::setMode(ControlMode mode)
{
    _mode = mode;

    bool isOffState = _mode == ControlMode::off;
    bool isScheduleState = _mode == ControlMode::schedule_check || _mode == ControlMode::schedule_fulfill;
    bool isOnState = _mode == ControlMode::on;
    
    digitalWrite(_isOffModeLedPin, isOffState);
    digitalWrite(_isScheduleModeLedPin, isScheduleState);
    digitalWrite(_isOnModeLedPin, isOnState);
}

void Controller::manage()
{
    switch (_mode)
    {
    case ControlMode::error... ControlMode::off:
        if (_isStarted)
        {
            stop();
        }
        break;

    case ControlMode::on:
        if (!_isStarted)
        {
            start();
        }
        break;

    case ControlMode::schedule_check:
        if (!_isReady)
        {
            if (_isStarted)
            {
                stop();
            }
            break;
        }

        _timeKeeper.tick();

        if (!shouldStart())
        {
            if (_isStarted)
            {
                stop();
            }
            break;
        }

        if (!_isStarted)
        {
            start();
            setMode(ControlMode::schedule_fulfill);
            break;
        }
        break;

    case ControlMode::schedule_fulfill:
        _timeKeeper.tick();
        if (shouldStop() && _isStarted)
        {
            stop();
            setMode(ControlMode::schedule_check);
            break;
        }
        break;

    default:
        break;
    }
}

void Controller::stop()
{
    control(false);
}

void Controller::start()
{
    control(true);
}

void Controller::control(bool state)
{
    digitalWrite(_relayPin, state);
    _isStarted = state;
    setIsReady(false);
}

void Controller::isOffModeBtnCallback(void *ptr)
{
    Controller *controllerPtr = (Controller *)ptr;
    controllerPtr->setMode(ControlMode::off);
}

void Controller::isScheduleModeBtnCallback(void *ptr)
{
    Controller *controllerPtr = (Controller *)ptr;
    controllerPtr->setMode(ControlMode::schedule_check);
}

void Controller::isOnModeBtnCallback(void *ptr)
{
    Controller *controllerPtr = (Controller *)ptr;
    controllerPtr->setMode(ControlMode::on);
}

void Controller::isReadyBtnCallback(void *ptr)
{
    Controller *controllerPtr = (Controller *)ptr;
    controllerPtr->setIsReady(!controllerPtr->_isReady);
}

void Controller::setIsReady(bool isReady)
{
    _isReady = isReady;
    digitalWrite(_isReadyLedPin, _isReady);
}

// TODO: Link to schedule
bool Controller::shouldStart()
{
    Serial.println("Evaluate shouldStart!");
    // Retrieve current time
    tm currentTimeInfo = _timeKeeper.currentTime();
    time_t currentTime = mktime(&currentTimeInfo);

    // Set start target time
    tm startTargetTimeInfo = currentTimeInfo;
    startTargetTimeInfo.tm_hour = (int)_targetTimeHours;
    startTargetTimeInfo.tm_min = (int)_targetTimeMinutes;
    startTargetTimeInfo.tm_sec = 0;
    time_t startTargetTime = mktime(&startTargetTimeInfo);

    // Set end target time
    tm endTargetTimeInfo = startTargetTimeInfo;
    endTargetTimeInfo.tm_min += (int)_keepWarmDuration;
    time_t endTargetTime = mktime(&endTargetTimeInfo);

    // Display info
    Serial.print("Current time - ");
    Serial.println(ctime(&currentTime));
    Serial.print("Start target time - ");
    Serial.println(ctime(&startTargetTime));
    Serial.print("End target time - ");
    Serial.println(ctime(&endTargetTime));

    // Evaluate scenario
    bool pastStartMoment = difftime(startTargetTime, currentTime) <= 0;
    bool pastFinishWarmingMoment = difftime(endTargetTime, currentTime) <= 0;

    // Display info
    Serial.print("pastStartMoment - ");
    Serial.println(pastStartMoment);
    Serial.print("pastFinishWarmingMoment - ");
    Serial.println(pastFinishWarmingMoment);

    // Evaluate result
    bool shouldStart = pastStartMoment && !pastFinishWarmingMoment;

    // Display info
    Serial.print("shouldStart - ");
    Serial.println(shouldStart);

    return shouldStart;
}

// TODO: Link to Schedule
bool Controller::shouldStop()
{
    Serial.println("Evaluate shouldStop!");
    // Retrieve current time
    tm currentTimeInfo = _timeKeeper.currentTime();
    time_t currentTime = mktime(&currentTimeInfo);

    // Set end target time
    tm endTargetTimeInfo = currentTimeInfo;
    endTargetTimeInfo.tm_hour = (int)_targetTimeHours;
    endTargetTimeInfo.tm_min = (int)_targetTimeMinutes;
    endTargetTimeInfo.tm_sec = 0;
    endTargetTimeInfo.tm_min += (int)_keepWarmDuration;
    time_t endTargetTime = mktime(&endTargetTimeInfo);

    // Display info
    Serial.print("Current time - ");
    Serial.println(ctime(&currentTime));
    Serial.print("End target time - ");
    Serial.println(ctime(&endTargetTime));

    // Evaluate result
    bool shouldStop = difftime(endTargetTime, currentTime) <= 0;

    // Display info
    Serial.print("shouldStop - ");
    Serial.println(shouldStop);

    return shouldStop;
}