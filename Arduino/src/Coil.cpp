#include "Coil.h"
#include "Motor.h"
#include "Configuration.h"

/*_____ PRIVATE FONCTIONS _____ */

/******************************************************************************
 * brief   : Convert number of turns in steps.
 * details : Convert number of turns in steps from steps motor for winding.
 * _coilTurns, number of turns that must be winding.
 * return  : Return steps.
 ******************************************************************************/
static unsigned long totalSteps(int coilTurns)
{
  return coilTurns * M1_STEPS_PER_TR;
}

/******************************************************************************
 * brief   : Convert reduction ratio into delay.
 * details : If the ration is inferior to 1 we need to convert delay
 * to keep the good reduction ratio between motors in acceleration() function.
 * ratio, value of reduction "_ratio".
 * delayMotor, delay on the motor who is the reference.
 * return  : delay value.
 ******************************************************************************/
static unsigned long ratioToDelay(float ratio, unsigned long delayMotor)
{
  unsigned long result;

  if (ratio >= 1)
    {
      return result = ratio * delayMotor;
    }
  else
    {
      return result = delayMotor / ratio;
    }
}

/******************************************************************************
 * brief   : Run action without delay.
 * details : timer() function need to be used in a while() loop and
 * often refresh. When time reach timer() return true.
 * currentTime, time to function "micros()". Refresh as often as possible !
 * oldTime, pointer to the variable who contain the old value of _currentTime.
 * this value is modified when time reach.
 * delay, delay before you do instructions.
 * return  : True when time reach.
 ******************************************************************************/
static bool timer(unsigned long currentTime, unsigned long *oldTime, unsigned long delay)
{
  if (currentTime - *oldTime >= delay)
    {
      *oldTime = currentTime;
      return true;
    }
  return false;
}

/******************************************************************************
 * brief   : Acceleration ramp.
 * details :  Modify motors delay to Make a smooth acceleration movement.
 * Or a deceleration.
 * acc, if true acceleration, else deceleration.
 * delayMotorA, delay that will be applied on winding motor.
 * limitSpeed, delay limit for motors speed.
 * ratio, reduction ratio between motors.
 * delayMotorB, delay that will be applied on carriage motor.
 ******************************************************************************/
static void acceleration(bool acc, unsigned long *delayMotorA, unsigned long limitSpeed,
			 float ratio, unsigned long *delayMotorB)
{
  // if ratio is >= 1
  unsigned long *delayMotor_x = delayMotorA;
  unsigned long *delayMotor_y = delayMotorB;
  // else invert delay motor.
  if(ratio < 1)
    {
      delayMotor_x = delayMotorB;
      delayMotor_y = delayMotorA;
    }

  //Acceleration.
  if(acc)
    {
      if(*delayMotor_x > limitSpeed)
	{
	  *delayMotor_x -= ACC;
	  *delayMotor_y = ratioToDelay(ratio, (*delayMotor_x));
	}
    }
  // Deceleration.
  else
    {
      if(*delayMotor_x < limitSpeed)
	{
	  *delayMotor_x += ACC;
	  *delayMotor_y = ratioToDelay(ratio, (*delayMotor_x));
	}
    }

}




/*_____ CONSTRUCTOR _____ */

Coil::Coil() : motorWinding (M1_DIR, M1_STEP, M1_EN, M1_STEPS_PER_TR),
    motorCarriage (M2_DIR, M2_STEP, M2_EN, M2_STEPS_PER_TR),

    _coilLength(0),
    _wireSize(0),
    _coilTurns(0),

    _accDelay (2000),
    _maxSpeed (600),
    _minSpeed (1800),

    _ratio(0),
    _stepsPerLayer(0),

    _totalStepsCounter(0)
{}

Coil::~Coil(){

}

/*_____  PUBLIC FUNCTIONS _____*/

void Coil::begin()
{
  motorCarriage.begin();
  motorWinding.begin();
}

void Coil::setWinding(float coilLength, float wireSize, int coilTurns)
{
  _coilLength = coilLength;
  _wireSize   = wireSize;
  _coilTurns = coilTurns;

  /*____Calculation____*/
  // Number of steps for carriage motor advance, when coil (winding motor) make one turn.
  float pitchToSteps = (M2_STEPS_PER_TR * _wireSize) / LEAD_SCREW_PITCH;
  // Reduction ratio due, between motors.
  _ratio = M1_STEPS_PER_TR / pitchToSteps;
  // Steps for winding one layer.
  _stepsPerLayer = (_coilLength / _wireSize) * pitchToSteps;

}

void Coil::setSpeed(unsigned long accDelay, unsigned long maxSpeed, unsigned long minSpeed)
{
  _accDelay = accDelay;
  _maxSpeed = maxSpeed;
  _minSpeed = minSpeed;
}

float Coil::getValue()
{
  return _stepsPerLayer;
}

void Coil::run()
{
  unsigned long delayMotor_A = _minSpeed;
  unsigned long delayMotor_B = _minSpeed;
  unsigned long previousMicrosMotor_A = 0;
  unsigned long previousMicrosMotor_B = 0;
  unsigned long previousMicrosAcc = 0;

  int direction = CLOCK;

  unsigned long layerStepsCounter = 0;
  unsigned long accStepsCounter = 0;
  _totalStepsCounter = 0;

  while(_totalStepsCounter < totalSteps(_coilTurns))
    {
      while(layerStepsCounter < _stepsPerLayer)
	{
	  unsigned long currentMicros = micros();

	  if(timer(currentMicros, &previousMicrosAcc, _accDelay))
	    {
	      if(layerStepsCounter < 2000)
		{
		  acceleration(true, &delayMotor_A, _maxSpeed, _ratio, &delayMotor_B);
		}
	      else
		{
		  acceleration(false, &delayMotor_A, _minSpeed, _ratio, &delayMotor_B);
		}
	    }
	  if(timer(currentMicros, &previousMicrosMotor_A, delayMotor_A))
	    {
	      motorWinding.oneStep(CLOCK);
	      _totalStepsCounter++;
	    }
	  if(timer(currentMicros, &previousMicrosMotor_B, delayMotor_B))
	    {
	      motorCarriage.oneStep(direction);
	      layerStepsCounter++;
	    }
	}

      direction = !direction;
      layerStepsCounter = 0;

    }
}






