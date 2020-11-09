#include "Controller.h"

uint8_t relayPin = LED_BUILTIN;
uint8_t schedulePin = D1;
uint8_t readyPin = D7;
uint8_t onPin = D2;

Controller controller(relayPin, schedulePin, readyPin, onPin);

void setup()
{
  controller.setup();
  Serial.begin(9600);
}

void loop()
{
  controller.tick();

  Serial.print("Mode: ");
  Serial.print(controller.showMode());
  Serial.print(" - IsReady: ");
  Serial.print(controller.isReady());
  Serial.print(" - Time: ");
  Serial.print(millis());
  Serial.print(" - IsStarted: ");
  Serial.println(controller.isStarted());
}