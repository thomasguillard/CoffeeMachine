/*
  I2CButtonsMgr.h - Support Library to manage mode buttons.
*/

#include "Arduino.h"
#include "I2CButton.h"
#include "ControlMode.h"
#include "Controller.h"

#ifndef I2CButtonsMgr_h
#define I2CButtonsMgr_h

class I2CButtonsMgr
{
public:
  I2CButtonsMgr(I2CButton* offBtn,
                I2CButton* scheduleBtn,
                I2CButton* onBtn,
                I2CButton* readyBtn,
                Controller* controller);
  void setup();
  void tick();

private:
  I2CButton* _offBtn;
  I2CButton* _scheduleBtn;
  I2CButton* _onBtn;
  I2CButton* _readyBtn;
  Controller* _controller;
};

#endif