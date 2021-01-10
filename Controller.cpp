/*
  Controller.cpp - Support Library to control a coffee machine.
*/

#include "Arduino.h"
#include "Controller.h"
#include "ControlMode.h"

// Constructor
Controller::Controller(uint8_t relayPin,
                       WiFiLink wifiLink,
                       TimeKeeper timeKeeper,
                       uint8_t targetTimeHours,
                       uint8_t targetTimeMinutes,
                       uint8_t keepWarmDuration)
{
    _relayPin = relayPin;
    _wiFiLink = wifiLink;
    _timeKeeper = timeKeeper;
    _targetTimeHours = targetTimeHours;
    _targetTimeMinutes = targetTimeMinutes;
    _keepWarmDuration = keepWarmDuration;
}

// Public
void Controller::setup()
{
    pinMode(_relayPin, OUTPUT);

    stop();
    setMode(ControlMode::off);

    _wiFiLink.setup();
    _timeKeeper.setup();
}

void Controller::tick()
{
    manage();
}

char* Controller::showMode()
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

ControlMode Controller::currentMode()
{
    return _mode;
}

void Controller::toggleIsReady()
{
    setIsReady(!_isReady);
}

void Controller::setMode(ControlMode mode)
{
    _mode = mode;
}

// Private
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
    digitalWrite(_relayPin, !state);
    _isStarted = state;
    setIsReady(false);
}

void Controller::setIsReady(bool isReady)
{
    _isReady = isReady;
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