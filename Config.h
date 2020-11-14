/*
  Config.h - Stores user configuration values.
*/

#include <arduino.h>

uint8_t config_relayPin;
uint8_t config_schedulePin;
uint8_t config_readyPin;
uint8_t config_onPin;
char *config_ssid;
char *config_password;
char *config_ntpServer;
char *config_timeZone;
uint16_t config_timeRefreshInterval;
uint8_t config_targetTimeHours;
uint8_t config_targetTimeMinutes;
uint8_t config_keepWarmDuration;