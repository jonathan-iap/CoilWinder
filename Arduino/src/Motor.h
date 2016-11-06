#ifndef MOTOR_H
#define MOTOR_H

#include "Arduino.h"

class Motor
{
public:

  Motor();
  Motor(int dirPin, int stepPin, int enablePin, int stepPerTr);

  ~Motor();

  void begin();
  void enable();
  void disable();
  void oneStep(int direction);

private:

  int _dirPin;
  int _stepPin;
  int _enablePin;
  int _stepsPerTr;

};

#endif
