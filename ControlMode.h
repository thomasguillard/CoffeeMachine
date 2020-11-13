#ifndef CONTROL_MODE_H
#define CONTROL_MODE_H

#include "Arduino.h"

enum ControlMode
{
  error,
  off,
  schedule_check,
  schedule_fulfill,
  on
};

#endif /* CONTROL_MODE_H */