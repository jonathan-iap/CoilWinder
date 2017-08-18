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

  bool computeWinding(float coilLength, float wireSize, uint16_t *nbTrForOneLayer, uint16_t *stepsPerTr );
  void setWinding(float coilLength, float wireSize, uint16_t coilTurns, bool windSense, bool startSense);
  void setSpeed(uint16_t accIncr, uint16_t accDelay, uint16_t maxSpeed, uint16_t minSpeed, uint16_t speed, int8_t speedPercent);
  void setSteps(uint16_t carrPass, uint16_t carrStepPerPass, uint16_t coilTr ,uint16_t coilStepPerTr);
  bool updateSpeed(int8_t *oldPercent, uint16_t *speedSet, uint8_t offset);

  bool winding(bool isNewCoil, float *homingPosition);
  void runOnlyCarriage(bool dir, float distance, float *homingPosition);
  void runOnlyCoil(bool dir, uint16_t turns);

  bool suspend();

  uint16_t getCurrentTurns();
  void getState(uint16_t *p_carrPass, uint16_t *p_carrSteps, uint16_t *p_coilTr, uint16_t *p_coilSteps);


private:

  //  void computeStepPerLayer(float length);
  //  void computeRatio();
  //  void computeStepsTravel(float totalSteps);
  //  void computeAll();
  //
  //  void homing(bool dir);

  void computeTravel(float distance, uint16_t *nbPass, uint16_t *stepsPerTr);
  void acceleration(uint16_t speedSet, uint16_t *accSpeed, uint32_t *oldTime);
  void refreshDisplay(bool *run, uint32_t *oldTime);
  float getRelativeHome(float homePosition, bool dir);

private:

  ClickEncoder *_Encoder;
  Display *_Display;

  //  Motor stepper;

  // Coil length in mm.
  float _coilLength;
  // Wire size in mm.
  float _wireSize;
  // Coil turn in turn.
  uint16_t _coilTurns;
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
  // Steps
  uint16_t _saveCarrPass;
  uint16_t _saveCarrStepPerPass;
  uint16_t _saveCoilTr;
  uint16_t _saveCoilStepPerTr;
};

#endif
