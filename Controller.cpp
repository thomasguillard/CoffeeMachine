/*
  Controller.cpp - Support Library to control a coffee machine.
*/

#include "Arduino.h"
#include "Controller.h"
#include "ControlMode.h"

// Constructor /////////////////////////////////////////////////////////////////
Controller::Controller(int relayPin, int isSchedulePin, int isOnPin)
{
    _mode = off;
    _isStarted = false;
    _relayPin = relayPin;
    _isSchedulePin = isSchedulePin;
    _isOnPin = isOnPin;
}

// Public Methods //////////////////////////////////////////////////////////////
void Controller::setup()
{
    pinMode(_isSchedulePin, INPUT_PULLUP);
    pinMode(_isOnPin, INPUT_PULLUP);
    pinMode(_relayPin, OUTPUT);
    stop();
}

void Controller::tick()
{
    evaluateMode();
    set();
}

ControlMode Controller::showMode()
{
    return _mode;
}

// Private Methods /////////////////////////////////////////////////////////////
void Controller::evaluateMode()
{
    bool isSchedule = !digitalRead(_isSchedulePin);
    bool isOn = !digitalRead(_isOnPin);

    // Check for potential shorts
    if (isSchedule and isOn)
    {
        _mode = error;
        return;
    }

    if (isOn)
    {
        _mode = on;
    }
    else if (isSchedule)
    {
        _mode = schedule;
    }
    else
    {
        _mode = off;
    }
}

void Controller::set()
{
    switch (_mode)
    {
    case error ... off:
        if (_isStarted)
        {
            stop();
        }
        break;

    case on:
        if (!_isStarted)
        {
            start();
        }
        break;

    case schedule:
        if (!_isStarted)
        {
            if (shouldStart())
            {
                start();
            }
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
}

bool Controller::shouldStart()
{
    return millis()>30000;
}