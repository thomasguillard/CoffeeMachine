/*
  TimeKeeper.h - Support Library to keep track of time.
*/

#include <ESP8266WiFi.h>
#include <time.h>

#ifndef TimeKeeper_h
#define TimeKeeper_h

class TimeKeeper
{
public:
    TimeKeeper();
    TimeKeeper(char *ntpServer, char *timeZone, int refreshInterval);
    void setup();
    void tick();
    tm currentTime();

private:
    char *_ntpServer;
    char *_timeZone;
    int _refreshInterval;
    unsigned long _lastCheckTime;
    tm _timeInfo;
    time_t _time;
    void fetchTime();
};

#endif