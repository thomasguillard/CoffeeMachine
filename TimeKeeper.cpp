/*
  TimeKeeper.cpp - Support Library to keep track of time.
*/

#include "Arduino.h"
#include "TimeKeeper.h"
#include <ESP8266WiFi.h>
#include <time.h>

// Constructors
TimeKeeper::TimeKeeper() {}
TimeKeeper::TimeKeeper(char *ntpServer, char *timeZone, int refreshInterval)
{
    _ntpServer = ntpServer;
    _timeZone = timeZone;
    _refreshInterval = refreshInterval;
    _lastCheckTime = 0;
}

// Public
void TimeKeeper::setup()
{
    configTime(0, 0, _ntpServer);
    setenv("TZ", _timeZone, 1);
    fetchTime();
    fetchTime();
}

void TimeKeeper::tick()
{
    const unsigned long currentCheckTime = millis();
    const unsigned long timeSinceLastCheck = currentCheckTime - _lastCheckTime;
    const bool updateRequired = timeSinceLastCheck >= _refreshInterval;
    if (updateRequired)
    {
        Serial.println("Updating time:");
        _lastCheckTime = currentCheckTime;
        fetchTime();
    }
}

tm TimeKeeper::currentTime()
{
    return _timeInfo;
}

// Private
void TimeKeeper::fetchTime()
{
    time(&_time);
    localtime_r(&_time, &_timeInfo);
}