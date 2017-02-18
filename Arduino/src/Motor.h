#ifndef MOTOR_H
#define MOTOR_H

#include "Arduino.h"

class Motor
{
public:

  Motor();
  Motor(uint8_t dirPin, uint8_t stepPin, uint8_t enablePin, uint16_t stepPerTr);

  ~Motor();

  void begin();
  void enable();
  void disable();
  void oneStep(bool direction);

private:

  uint8_t _dirPin;
  uint8_t _stepPin;
  uint8_t _enablePin;
  uint8_t _stepsPerTr;

};

#endif
