#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

class Motor
{
  public:;
  Motor();
  Motor(int _dirPin, int _stepPin, int _enablePin, int _stepPerTr);
  ~Motor();
  void init();
  bool timer(unsigned long _currentTime);
  void oneStep();
  bool acceleration(bool _acc, float _ratio);
  float steppingTravel();

  // fonctions


  private:
  int dirPin;
  int stepPin;
  int enablePin;
  int stepsPerTr;

  unsigned long delayMotor;// = 1800;
  unsigned long previousMicrosMotor;// = 0
  unsigned long previousMicrosAcc; // 0;
  unsigned long accDelay;// = 2000;
  unsigned long maxSpeed;// = 600;
  unsigned long minSpeed;// = 1800;
  bool acc;// = true;

};

#endif
