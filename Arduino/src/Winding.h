#ifndef WINDING_H
#define WINDING_H

#include "Arduino.h"
#include "Motor.h"
#include "Configuration.h"
#include "Function.h"
#include "Display.h"
#include "ClickEncoder.h"

#define ACCELERATION 	true
#define DECELERATION 	false
#define UPDATE		true

class Coil
{
public:

  Coil(ClickEncoder *p_Encoder, Display *p_Display);
  ~Coil();

  void setWinding(float coilLength, float wireSize, uint32_t coilTurns, bool windSense, bool startSense);
  void setSpeed(uint16_t accDelay, uint16_t maxSpeed, uint16_t minSpeed, uint16_t speed);
  void setSteps(uint32_t totalSteps, uint32_t layerSteps, uint32_t coilSteps);
  void updateSpeed(uint16_t speed);

  bool runMultiLayer(bool isNewCoil);
  bool runOneLayer();
  bool runOnlyCarriage(bool dir, float distance);
  bool runOnlyCoil(bool dir, uint32_t turns);

  bool suspend();
  void disableMotors();

  uint32_t getTurns();
  uint32_t getCurrentTurns();
  uint32_t getTotalStepsCounter();
  uint32_t getLayerStepsCounter();
  uint32_t getLayerCoilStepsCounter();

private:

  void computeStepPerLayer(float length);
  void computeRatio();
  void computeStepsTravel(float totalSteps);
  void computeAll();

  void homing(bool dir);

private:

  ClickEncoder *_Encoder;
  Display *_Display;

  Motor stepper;

  // Coil length in mm.
  float _coilLength;
  // Wire size in mm.
  float _wireSize;
  // Coil turn in turn.
  uint32_t _coilTurns;
  // Winding sense.
  bool _windingSense;
  bool _carriageStartSense;
  // Speed.
  uint16_t _accDelay;
  uint16_t _maxSpeed;
  uint16_t _minSpeed;
  uint16_t _speed;
  // Reduction ratio for motor.
  float _ratio;
  bool _direction;
  // Steps for one layer.
  uint32_t _stepsPerLayer;
  uint32_t _stepsCoilPerLayer;
  uint32_t _stepsTravel;
  uint32_t _totalStepsCounter;
  uint32_t _layerStepsCounter;
  uint32_t _layerCoilStepsCounter;

};

#endif
