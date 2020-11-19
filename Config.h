/*
  Config.h - Stores user configuration values.
*/

#include <arduino.h>

uint8_t config_relayPin;
uint8_t config_isOffModeBtnPin;
uint8_t config_isOffModeLedPin;
uint8_t config_isScheduleModeBtnPin;
uint8_t config_isScheduleModeLedPin;
uint8_t config_isOnModeBtnPin;
uint8_t config_isOnModeLedPin;
uint8_t config_isReadyBtnPin;
uint8_t config_isReadyLedPin;
char *config_ssid;
char *config_password;
char *config_ntpServer;
char *config_timeZone;
uint16_t config_timeRefreshInterval;
uint8_t config_targetTimeHours;
uint8_t config_targetTimeMinutes;
uint8_t config_keepWarmDuration;