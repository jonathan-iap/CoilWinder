#ifndef WINDING_H
#define WINDING_H

#include "Arduino.h"
#include "Motor.h"
#include "Configuration.h"
#include "Function.h"

class Coil
{
public:

  Coil();
  ~Coil();

  void getWinding(float coilLength, float wireSize, unsigned long coilTurns);
  void getSpeed(unsigned long accDelay, unsigned long maxSpeed, unsigned long minSpeed);
  void computeRatio();
  void computeStepsTravel();
  void computeAll();
  float getValue();

  void run();
  void oneLayer(bool dir, bool M_carriage, bool M_winding, unsigned long *p_totalStepsCounter);
  void stopMotion();
  void disableMotors();

private:

  Motor motorWinding, motorCarriage;

  // Coil length in mm.
  float _coilLength;
  // wire size in mm.
  float _wireSize;
  //coil turn in turn.
  unsigned long _coilTurns;

  unsigned long _accDelay;
  unsigned long _maxSpeed;
  unsigned long _minSpeed;

  // reduction ratio for motor.
  float _ratio;
  // steps for one layer.
  unsigned long _stepsPerLayer;
  unsigned long _stepsTravel;

};

#endif
