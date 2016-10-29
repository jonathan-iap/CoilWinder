#include <Coil.h>
#include "Configuration.h"


//Coil::Coil(Motor & _mA, Motor & _mB){
Coil::Coil(){
  //mB = new Motor (M2_DIR, M2_STEP, M2_EN, M2_STEPS_PER_TR);
  
  //Motor mA (M1_DIR, M1_STEP, M1_EN, M1_STEPS_PER_TR); //  ça marche avec déclaration dans .h : "Motor mA" avec mA.init();
 // mA = new Motor (M1_DIR, M1_STEP, M1_EN, M1_STEPS_PER_TR); // ça marche avec déclaration dans .h : "Motor * mA" et mA->init();
  
  Motor mA (M1_DIR, M1_STEP, M1_EN, M1_STEPS_PER_TR);
  Motor mB (M2_DIR, M2_STEP, M2_EN, M2_STEPS_PER_TR);

//  mA = _mA;
//  mB = _mB;

}

Coil::~Coil(){
  
}

void Coil::init(float _coilLength, float _wireSize, int _coilTurns){
    coilLength = _coilLength;
    wireSize   = _wireSize;
    coilTurns = _coilTurns;
    stepsCounter = 0;
    stepsLayerCounter = 0;
    
    stepsEnd = coilTurnsToSteps();
    pitchSteps = pitchToSteps();
    reductionRatio = reduction();
    stepsPerLayer = lengthToSteps();

    mA.init();
    mB.init();
}


void Coil::go(){
  
} 


bool Coil::isFinished(){
  return stepsCounter <= stepsEnd;
} 





// Calculs
unsigned long Coil::coilTurnsToSteps()
{
  return coilTurns * M1_STEPS_PER_TR;
}

float Coil::pitchToSteps(){
  return (M2_STEPS_PER_TR * wireSize) / LEAD_SCREW_PITCH;
}

float Coil::reduction(){
  return  M1_STEPS_PER_TR / pitchSteps;
}

float Coil::lengthToSteps (){
  return (coilLength / wireSize) * pitchSteps;
}
