#include "Config.h"
#include "I2CButtonsMgr.h"
#include "I2CButton.h"
#include "Controller.h"
#include "PCF8574.h"
#include "RequestsMgr.h"

Controller controller(config_relayPin,
                      WiFiLink(config_ssid,
                               config_password));

PCF8574 I2CBus(config_I2CBusAddress);

I2CButton offButton(&I2CBus,
                    config_isOffModeBtnPin,
                    config_isOffModeLedPin);

I2CButton scheduleButton(&I2CBus,
                         config_isScheduleModeBtnPin,
                         config_isScheduleModeLedPin);

I2CButton onButton(&I2CBus,
                   config_isOnModeBtnPin,
                   config_isOnModeLedPin);

I2CButton readyButton(&I2CBus,
                      config_isReadyBtnPin,
                      config_isReadyLedPin);

I2CButtonsMgr buttonsMgr(&offButton,
                         &scheduleButton,
                         &onButton,
                         &readyButton,
                         &controller);

RequestsMgr requestsMgr(config_serverPort, 
                        &controller);

void setup()
{
  Serial.begin(115200);
  controller.setup();
  controller.setSchedule(config_targetTimeHours,
                         config_targetTimeMinutes, 
                         config_keepWarmDuration);
  I2CBus.begin();
  buttonsMgr.setup();
  requestsMgr.setup();
}

void loop()
{
  controller.tick();
  buttonsMgr.tick();
  requestsMgr.tick();
}