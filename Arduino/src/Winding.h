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

  void begin();
  void setWinding(float coilLength, float wireSize, unsigned long coilTurns);
  void setSpeed(unsigned long accDelay, unsigned long maxSpeed, unsigned long minSpeed);
  float getValue();

  void run();
  void oneLayer(float _coilLength, int _dir);

private:

  Motor motorWinding, motorCarriage;

  // Coil length in mm.
  float _coilLength;
  // wire size in mm.
  float _wireSize;
  //coil turn in turn.
  unsigned long _coilTurns;

  unsigned long _accDelay = 2000;
  unsigned long _maxSpeed = 600;
  unsigned long _minSpeed = 1800;

  // reduction ratio for motor.
  float _ratio;
  // steps for one layer.
  unsigned long _stepsPerLayer;
  unsigned long _stepsTravel;

  // counter.
  unsigned long _totalStepsCounter;
};

#endif
