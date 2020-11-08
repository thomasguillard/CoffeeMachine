/*
  Controller.h - Support Library to control a coffee machine.
*/
#include "ControlMode.h"

#ifndef Controller_h
#define Controller_h

class Controller
{
public:
    Controller(int relayPin, int isSchedulePin, int isOnPin);
    void setup();
    void tick();
    ControlMode showMode();

private:
    ControlMode _mode;
    int _relayPin;
    int _isSchedulePin;
    int _isOnPin;
    bool _isStarted;
    void evaluateMode();
    void set();
    void stop();
    void start();
    void control(bool state);
    bool shouldStart();
};

#endif