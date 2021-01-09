/*
  I2CButtonsMgr.cpp - Support Library to manage mode buttons.
*/

#include "I2CButtonsMgr.h"

// Constructors
I2CButtonsMgr::I2CButtonsMgr(I2CButton *offBtn,
                             I2CButton *scheduleBtn,
                             I2CButton *onBtn,
                             I2CButton *readyBtn,
                             Controller *controller)
    : _offBtn(offBtn),
      _scheduleBtn(scheduleBtn),
      _onBtn(onBtn),
      _readyBtn(readyBtn),
      _controller(controller)
{
}

// Public
void I2CButtonsMgr::setup()
{
  _offBtn->setup();
  _scheduleBtn->setup();
  _onBtn->setup();
  _readyBtn->setup();
}

void I2CButtonsMgr::tick()
{
  // Get current states
  ControlMode ctrlrMode = _controller->currentMode();
  bool ctrlrReady = _controller->isReady();

  // Set Leds from states
  _offBtn->setLed(ctrlrMode == ControlMode::off);
  _scheduleBtn->setLed(ctrlrMode == ControlMode::schedule_check || ctrlrMode == ControlMode::schedule_fulfill);
  _onBtn->setLed(ctrlrMode == ControlMode::on);
  _readyBtn->setLed(ctrlrReady);

  // Retrieve button states
  _offBtn->tick();
  _scheduleBtn->tick();
  _onBtn->tick();
  _readyBtn->tick();

  // Check for pressed buttons
  if (_offBtn->fire())
  {
    _controller->setMode(ControlMode::off);
  }
  if (_scheduleBtn->fire())
  {
    _controller->setMode(ControlMode::schedule_check);
  }
  if (_onBtn->fire())
  {
    _controller->setMode(ControlMode::on);
  }
  if (_readyBtn->fire())
  {
    _controller->toggleIsReady();
  }
}