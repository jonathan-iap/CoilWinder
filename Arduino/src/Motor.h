#ifndef MOTOR_H
#define MOTOR_H

#include "Arduino.h"

class Motor
{
public:

  Motor();
  ~Motor();

  void begin();

  void coil_enable();
  void coil_disable();
  void coil_oneStep(bool direction);

  void carriage_enable();
  void carriage_disable();
  void carriage_oneStep(bool direction);
};

#endif
