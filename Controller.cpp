/*
  Controller.cpp - Support Library to control a coffee machine.
*/

#include "Arduino.h"
#include "Controller.h"
#include "ControlMode.h"

// Constructor
Controller::Controller(uint8_t relayPin,
                       WiFiLink wifiLink)
{
    _relayPin = relayPin;
    _wiFiLink = wifiLink;
}

// Public
void Controller::setup()
{
    pinMode(_relayPin, OUTPUT);

    stop();
    setMode(ControlMode::off);

    _wiFiLink.setup();
    _rtc.begin();
}

void Controller::tick()
{
    manage();
}

char *Controller::showMode()
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

void Controller::setSchedule(uint8_t hours, uint8_t minutes, uint8_t keepWarmDuration)
{
    DateTime now = _rtc.now();
    _startDateTime = DateTime(now.year(),
                              now.month(),
                              now.day(),
                              hours,
                              minutes,
                              0);
    _endDateTime = _startDateTime + TimeSpan(keepWarmDuration * 60);
}

char *Controller::getSchedule()
{
    TimeSpan keepWarm = _endDateTime - _startDateTime;

    char res[1000];
    snprintf(res,
             sizeof(res),
             "%d:%d for %dh, %dm",
             _startDateTime.hour(),
             _startDateTime.minute(),
             keepWarm.hours(),
             keepWarm.minutes());

    return res;
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
    Serial.println("Evaluate shouldStart:");
    Serial.println("---------------------");

    DateTime now = _rtc.now();

    // Display info
    Serial.print("> Current time: ");
    Serial.println(now.timestamp());
    Serial.print("> Start target time: ");
    Serial.println(_startDateTime.timestamp());
    Serial.print("> End target time: ");
    Serial.println(_endDateTime.timestamp());

    // Evaluate scenario
    bool pastStartMoment = now >= _startDateTime;
    bool pastFinishWarmingMoment = now >= _endDateTime;

    // Display info
    Serial.print("-> pastStartMoment: ");
    Serial.println(pastStartMoment);
    Serial.print("-> pastFinishWarmingMoment: ");
    Serial.println(pastFinishWarmingMoment);

    // Evaluate result
    bool shouldStart = pastStartMoment && !pastFinishWarmingMoment;

    // Display info
    Serial.print("=> shouldStart: ");
    Serial.println(shouldStart);
    Serial.println();

    return shouldStart;
}

// TODO: Link to Schedule
bool Controller::shouldStop()
{
    Serial.println("Evaluate shouldStop:");
    Serial.println("--------------------");

    DateTime now = _rtc.now();

    // Display info
    Serial.print("> Current time: ");
    Serial.println(now.timestamp());

    Serial.print("> End target time: ");
    Serial.println(_endDateTime.timestamp());

    // Evaluate result
    bool shouldStop = now >= _endDateTime;

    // Display info
    Serial.print("=> Should stop: ");
    Serial.println(shouldStop);
    Serial.println();

    return shouldStop;
}