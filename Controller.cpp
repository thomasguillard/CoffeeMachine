/*
  Controller.cpp - Support Library to control a coffee machine.
*/

#include "Arduino.h"
#include "Controller.h"
#include "ControlMode.h"
#include <OneButton.h>

// Constructor /////////////////////////////////////////////////////////////////
Controller::Controller(uint8_t  relayPin, uint8_t  isSchedulePin, uint8_t  isReadyPin, uint8_t  isOnPin)
{
    _relayPin = relayPin;
    _isSchedulePin = isSchedulePin;
    _isReadyPin = isReadyPin;
    _isOnPin = isOnPin;
}

// Public Methods //////////////////////////////////////////////////////////////
void Controller::setup()
{
    pinMode(_relayPin, OUTPUT);
    pinMode(_isSchedulePin, INPUT_PULLUP);
    pinMode(_isOnPin, INPUT_PULLUP);

    _btn = OneButton(_isReadyPin, true, true);
    _btn.attachClick(btnCallback, this);

    stop();
}

void Controller::tick()
{
    evaluateMode();
    _btn.tick();
    set();
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

    case ControlMode::schedule:
        return "Schedule";
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

// Private Methods /////////////////////////////////////////////////////////////
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
        _mode = ControlMode::schedule;
    }
    else
    {
        _mode = ControlMode::off;
    }
}

void Controller::set()
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

    case ControlMode::schedule:
        if (!_isStarted && _isReady && shouldStart())
        {
            start();
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

// TODO: Link to schedule
bool Controller::shouldStart()
{
    return millis() > 30000;
}