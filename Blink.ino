#include "ControlMode.h"

int schedulePin = D1;
int onPin = D2;
int relayPin = LED_BUILTIN;

ControlMode mode = off;
bool isCoffeeStarted = false;

void setup()
{
  pinMode(schedulePin, INPUT_PULLUP);
  pinMode(onPin, INPUT_PULLUP);
  pinMode(relayPin, OUTPUT);
  stopCoffee();
  Serial.begin(9600);
}

void loop()
{
  evaluateMode();
  modeControl();

  Serial.print("Mode: ");
  Serial.print(mode);
  Serial.print(" - Time: ");
  Serial.println(millis());
}

void evaluateMode()
{
  bool isSchedule = !digitalRead(schedulePin);
  bool isOn = !digitalRead(onPin);

  // Check for potential shorts
  if (isSchedule and isOn)
  {
    mode = error;
    return;
  }

  if (isOn)
  {
    mode = on;
  }
  else if (isSchedule)
  {
    mode = schedule;
  }
  else
  {
    mode = off;
  }
}

void modeControl()
{
  switch (mode)
  {
  case error ... off:
    if (isCoffeeStarted)
    {
      stopCoffee();
    }
    break;

  case on:
    if (!isCoffeeStarted)
    {
      startCoffee();
    }
    break;

  case schedule:
    if (!isCoffeeStarted)
    {
      if (shouldStart())
      {
        startCoffee();
      }
    }
    break;

  default:
    break;
  }
}

void startCoffee()
{
  controlCoffee(true);
}

void stopCoffee()
{
  controlCoffee(false);
}

void controlCoffee(bool state)
{
  setRelay(state);
  isCoffeeStarted = state;
}

void setRelay(bool state)
{
  digitalWrite(relayPin, !state);
}

bool shouldStart()
{
  // TODO: check scheduling
  return millis() > 30000;
}