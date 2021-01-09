/*
  I2CButton.h - Support Library to manage a button with an LED over I2C.
*/

#include "Arduino.h"
#include <PCF8574.h>

#ifndef I2CButton_h
#define I2CButton_h

class I2CButton
{
public:
  I2CButton(PCF8574* I2CBus,
            uint8_t btnPin,
            uint8_t ledPin);
  void setup();
  void tick();
  void setLed(bool state);
  bool fire();

private:
  PCF8574* _I2CBus;
  uint8_t _btnPin;
  uint8_t _ledPin;
  bool _lastPressed;
  bool _fire;
};

#endif