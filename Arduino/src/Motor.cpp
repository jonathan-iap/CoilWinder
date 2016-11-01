#include "Motor.h"
#include "Configuration.h"

Motor::Motor() : _dirPin(0),_stepPin(0),_enablePin(0),_stepsPerTr(0)
{}

Motor::Motor(int dirPin, int stepPin, int enablePin, int stepPerTr) :
      _dirPin (dirPin),
      _stepPin (stepPin),
      _enablePin (enablePin),
      _stepsPerTr (stepPerTr)
{}

Motor::~Motor()
{}

void Motor::begin()
{
  pinMode(_dirPin, OUTPUT);
  pinMode(_stepPin, OUTPUT);
  pinMode(_enablePin, OUTPUT);

  digitalWrite(_enablePin, DISABLE);
}


void Motor::enable()
{
  digitalWrite(_enablePin, ENABLE);
}


void Motor::disable()
{
  digitalWrite(_enablePin, DISABLE);
}

/******************************************************************************
 * brief   : Motor advance.
 * details : Verified that enable pin is ok and make one step in the direction
 * you want.
 * _enablePin, pin of motor that you want drive.
 * direction, set direction.
 * _stepPin, pin of motor that you want drive.
 * return  : nothing.
 ******************************************************************************/
void Motor::oneStep(int direction)
{
  if(digitalRead(_enablePin) != ENABLE)
    {
      enable();
    }

  if(digitalRead(_dirPin) != direction)
    {
      digitalWrite(_dirPin, direction);
    }

  digitalWrite(_stepPin, HIGH);
#ifdef  DELAY_DRIVER
  delayMicroseconds(DELAY_DRIVER);
#endif
  digitalWrite(_stepPin, LOW);
}
