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

  void setWinding(float coilLength, float wireSize, uint32_t coilTurns);
  void setSpeed(uint16_t accDelay, uint16_t maxSpeed, uint16_t minSpeed, uint16_t speed);
  void setSteps(uint32_t totalSteps, uint32_t layerSteps);
  void updateSpeed(uint16_t speed);

  bool runMultiLayer(bool isNewCoil);
  bool runOneLayer();
  void runOnlyCarriage(bool dir, float distance);
  void runOnlyCoil(bool dir, uint32_t turns);

  bool suspend();
  void disableMotors();

  uint32_t getTurns();
  uint32_t getCurrentTurns();
  uint32_t getTotalStepsCounter();
  uint32_t getLayerStepsCounter();

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
  // wire size in mm.
  float _wireSize;
  //coil turn in turn.
  uint32_t _coilTurns;

  uint16_t _accDelay;
  uint16_t _maxSpeed;
  uint16_t _minSpeed;
  uint16_t _speed;

  // reduction ratio for motor.
  float _ratio;
  bool _direction;
  // steps for one layer.
  uint32_t _stepsPerLayer;
  uint32_t _stepsTravel;
  uint32_t _totalStepsCounter;
  uint32_t _layerStepsCounter;

};

#endif
