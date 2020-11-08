#include "Controller.h"

int relayPin = LED_BUILTIN;
int schedulePin = D1;
int onPin = D2;

Controller controller(relayPin, schedulePin, onPin);

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
  Serial.print(" - Time: ");
  Serial.println(millis());
}