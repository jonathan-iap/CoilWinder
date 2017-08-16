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
#define MAX_INTEGER 	65535

class Coil
{
public:

  Coil(ClickEncoder *p_Encoder, Display *p_Display);
  ~Coil();

  void setWinding(float coilLength, float wireSize, uint32_t coilTurns, bool windSense, bool startSense);
  void setSpeed(uint16_t accIncr, uint16_t accDelay, uint16_t maxSpeed, uint16_t minSpeed, uint16_t speed, int8_t speedPercent);
  void setSteps(uint32_t totalSteps, uint32_t layerSteps, uint32_t coilSteps);
  void updateSpeed(int8_t *oldPercent, uint16_t *speedSet);

//  bool runMultiLayer(bool isNewCoil);
//  bool runOneLayer();
  bool runOnlyCarriage(bool dir, float distance);
  bool runOnlyCoil(bool dir, uint16_t turns);

  bool suspend();
//  void disableMotors();

//  uint32_t getTurns();
//  uint32_t getCurrentTurns();
//  uint32_t getTotalStepsCounter();
//  uint32_t getLayerStepsCounter();
//  uint32_t getLayerCoilStepsCounter();

private:

//  void computeStepPerLayer(float length);
//  void computeRatio();
//  void computeStepsTravel(float totalSteps);
//  void computeAll();
//
//  void homing(bool dir);

  void computeTravel(float distance, uint16_t *nbPass, uint16_t *stepsPerTr);
  void acceleration(uint16_t speedSet, uint16_t *accSpeed, uint32_t *oldTime);

private:

  ClickEncoder *_Encoder;
  Display *_Display;

  //  Motor stepper;

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
  uint16_t _accIncr;
  uint16_t _accDelay;
  uint16_t _maxSpeed;
  uint16_t _minSpeed;
  uint16_t _speed;
  int8_t _speedPercent;
  // Reduction ratio for motor.
  float _ratio;
  bool _direction;
  // Steps for one layer.

};

#endif
