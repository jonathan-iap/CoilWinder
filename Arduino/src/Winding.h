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
  void getWinding(double coilLength, double wireSize, unsigned long coilTurns);
  void getSpeed(unsigned long accDelay, unsigned long maxSpeed, unsigned long minSpeed);
  void computing();
  double getValue();

  void run();
  void oneLayer(bool dir, bool M_carriage, bool M_winding, unsigned long *p_totalStepsCounter);
  void disableMotors();

private:

  Motor motorWinding, motorCarriage;

  // Coil length in mm.
  double _coilLength;
  // wire size in mm.
  double _wireSize;
  //coil turn in turn.
  unsigned long _coilTurns;

  unsigned long _accDelay = 2000;
  unsigned long _maxSpeed = 600;
  unsigned long _minSpeed = 1800;

  // reduction ratio for motor.
  double _ratio;
  // steps for one layer.
  unsigned long _stepsPerLayer;
  unsigned long _stepsTravel;

};

#endif
