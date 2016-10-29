#ifndef Coil_H
#define Coil_H

#include <Arduino.h>
#include "Motor.h"


class Coil : public Motor
{
  public:
  Coil();
  //Coil(Motor & _mA, Motor & _mB);
  //Coil(Motor _mA, Motor _mB);
  ~Coil();
  void init(float _coilLength, float _wireSize, int _coilTurns);
  bool isFinished();
  
  unsigned long coilTurnsToSteps();
  float pitchToSteps();
  float reduction();
  float lengthToSteps();
  void go();


  private:
  Motor & mA;
  Motor & mB;

  
  float coilLength;;
  float wireSize;
  int coilTurns;
  float pitchSteps;
  float reductionRatio;
  float stepsPerLayer;

  unsigned long stepsEnd;
  unsigned long stepsCounter;
  unsigned long stepsLayerCounter;

};

#endif
