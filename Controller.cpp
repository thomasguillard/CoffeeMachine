/*
  Controller.cpp - Support Library to control a coffee machine.
*/

#include "Arduino.h"
#include "Controller.h"
#include "ControlMode.h"
#include <OneButton.h>

// Constructor
Controller::Controller(uint8_t relayPin,
                       uint8_t isSchedulePin,
                       uint8_t isReadyPin,
                       uint8_t isOnPin,
                       char *ssid,
                       char *password,
                       char *ntpServer,
                       char *timeZone,
                       int timeRefreshInterval)
{
    _relayPin = relayPin;
    _isSchedulePin = isSchedulePin;
    _isReadyPin = isReadyPin;
    _isOnPin = isOnPin;
    _wiFiLink = WiFiLink(ssid, password);
    _timeKeeper = TimeKeeper(ntpServer, timeZone, timeRefreshInterval);
}

// Public
void Controller::setup()
{
    pinMode(_relayPin, OUTPUT);
    pinMode(_isSchedulePin, INPUT_PULLUP);
    pinMode(_isOnPin, INPUT_PULLUP);

    _btn = OneButton(_isReadyPin, true, true);
    _btn.attachClick(btnCallback, this);

    stop();

    _wiFiLink.setup();
    _timeKeeper.setup();
}

void Controller::tick()
{
    _btn.tick();
    evaluateMode();
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
void Controller::evaluateMode()
{
    bool isSchedule = !digitalRead(_isSchedulePin);
    bool isOn = !digitalRead(_isOnPin);

    // Check for potential shorts
    if (isSchedule && isOn)
    {
        _mode = ControlMode::error;
        return;
    }

    if (isOn)
    {
        _mode = ControlMode::on;
    }
    else if (isSchedule)
    {
        if (_mode == ControlMode::schedule_check || _mode == ControlMode::schedule_fulfill)
        {
            _mode = _mode;
            return;
        }

        _mode = ControlMode::schedule_check;
    }
    else
    {
        _mode = ControlMode::off;
    }
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
            _mode = ControlMode::schedule_fulfill;
            break;
        }
        break;

    case ControlMode::schedule_fulfill:
        _timeKeeper.tick();
        if (shouldStop() && _isStarted)
        {
            stop();
            _mode = ControlMode::schedule_check;
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
    digitalWrite(_relayPin, !state);
    _isStarted = state;
    _isReady = false;
}

// Private
void Controller::btnCallback(void *ptr)
{
    Controller *controllerPtr = (Controller *)ptr;
    controllerPtr->_isReady = !controllerPtr->_isReady;
}

// TODO: Link to schedule and how long to keep coffee warm for.
bool Controller::shouldStart()
{
    tm time = _timeKeeper.currentTime();
    Serial.print("Current time - WD: ");
    Serial.print(time.tm_wday);
    Serial.print(" - ");
    Serial.print(time.tm_hour);
    Serial.print(":");
    Serial.print(time.tm_min);
    return time.tm_wday == 4 && time.tm_hour == 22 && time.tm_min >= 57 && time.tm_min < 58;
}

// TODO: Link to how long to keep coffee warm for.
bool Controller::shouldStop()
{
    tm time = _timeKeeper.currentTime();
    Serial.print("Current time - WD: ");
    Serial.print(time.tm_wday);
    Serial.print(" - ");
    Serial.print(time.tm_hour);
    Serial.print(":");
    Serial.print(time.tm_min);
    return time.tm_wday == 4 && time.tm_hour == 22 && time.tm_min >= 58;
}