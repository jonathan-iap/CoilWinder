#ifndef WINDING_H
#define WINDING_H

#include "Arduino.h"
#include "Motor.h"
#include "Configuration.h"
#include "Function.h"

#define ACCELERATION true
#define DECELERATION false

class Coil
{
public:

  Coil();
  ~Coil();

  void setWinding(float coilLength, float wireSize, unsigned long coilTurns);
  void setSpeed(unsigned long accDelay, unsigned long maxSpeed, unsigned long minSpeed);

  void runMultiLayer();
  void runOneLayer(bool dir, unsigned long *p_totalStepsCounter, unsigned long *p_layerStepsCounter);
  void runOnlyCarriage(bool dir, float distance);
  void runOnlyCoil(bool dir, float turns);
  void stopMotion();
  void disableMotors();

private:

  void computeStepPerLayer(float length);
  void computeRatio();
  void computeStepsTravel(float totalSteps);
  void computeAll();

  void homing(bool dir, unsigned long layerStepsCounter);

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
