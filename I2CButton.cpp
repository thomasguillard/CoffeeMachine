/*
  I2CButton.cpp - Support Library to manage a button with an LED over I2C.
*/

#include "I2CButton.h"

// Constructors
I2CButton::I2CButton(PCF8574 *I2CBus,
                     uint8_t btnPin,
                     uint8_t ledPin)
    : _I2CBus(I2CBus),
      _btnPin(btnPin),
      _ledPin(ledPin),
      _lastPressed(false),
      _fire(false)
{
}

// Public
void I2CButton::setup()
{
  _I2CBus->pinMode(_btnPin, INPUT);
  _I2CBus->pinMode(_ledPin, OUTPUT);
}

void I2CButton::tick()
{
  bool isPressed = _I2CBus->digitalRead(_btnPin, true);

  _fire = !_lastPressed && isPressed;
  _lastPressed = isPressed;
}

void I2CButton::setLed(bool state)
{
  _I2CBus->digitalWrite(_ledPin, state);
}

bool I2CButton::fire()
{
  return _fire;
}