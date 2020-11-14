#include "Controller.h"
#include "Config.h"

Controller controller(config_relayPin,
                      config_schedulePin,
                      config_readyPin,
                      config_onPin,
                      config_ssid,
                      config_password,
                      config_ntpServer,
                      config_timeZone,
                      config_timeRefreshInterval,
                      config_targetTimeHours,
                      config_targetTimeMinutes,
                      config_keepWarmDuration);

void setup()
{
  controller.setup();
  Serial.begin(115200);
}

void loop()
{
  controller.tick();

  Serial.print("Mode: ");
  Serial.print(controller.showMode());
  Serial.print(" - IsReady: ");
  Serial.print(controller.isReady());
  Serial.print(" - IsStarted: ");
  Serial.println(controller.isStarted());
}