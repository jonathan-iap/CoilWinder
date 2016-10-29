#include "Motor.h"
#include "Configuration.h"

Motor::Motor(){
}


Motor::Motor(int _dirPin, int _stepPin, int _enablePin, int _stepPerTr){
  dirPin = _dirPin;
  stepPin = _stepPin;
  enablePin = _enablePin;
  stepsPerTr = _stepPerTr;
  
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  
  digitalWrite(enablePin, DISABLE);
}

Motor::~Motor() {
  
}

void Motor::init(){
  delayMotor = 1800;
  previousMicrosMotor = 0;
  previousMicrosAcc = 0;
  accDelay = 2000;
  maxSpeed = 600;
  minSpeed = 1800;
  acc = true;
}

bool Motor::timer(unsigned long _currentTime){
  
}

void Motor::oneStep(){
  
}

bool Motor::acceleration(bool _acc, float _ratio){
  
}

float Motor::steppingTravel(){
  
}

  
