#include "Winding.h"

/*_____ PRIVATE FONCTIONS _____ */

/******************************************************************************
 * brief   : Convert number of turns in steps.
 * details : Convert number of turns in steps from steps motor for winding.
 * _coilTurns, number of turns that must be winding.
 * return  : Return steps.
 ******************************************************************************/
#define totalSteps(coilTurns) coilTurns * M1_STEPS_PER_TR

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

  ratio >= 1 ? result = ratio * delayMotor : result = delayMotor / ratio;

  return result;
  //  if (ratio >= 1){ return result = ratio * delayMotor; }
  //  else{ return result = delayMotor / ratio; }
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
      if(*delayMotor_x < limitSpeed )
	{
	  *delayMotor_x += ACC;
	  *delayMotor_y = ratioToDelay(ratio, (*delayMotor_x));
	}
    }
}




/*_____ CONSTRUCTOR _____ */

Coil::Coil()
: motorWinding (M1_DIR, M1_STEP, M1_EN, M1_STEPS_PER_TR),
  motorCarriage (M2_DIR, M2_STEP, M2_EN, M2_STEPS_PER_TR),

  _coilLength(0),
  _wireSize(0),
  _coilTurns(0),

  _accDelay (400),
  _maxSpeed (30),
  _minSpeed (1400),

  _ratio(0),
  _stepsPerLayer(0),
  _stepsTravel(0),

  _totalStepsCounter(0)
{}

Coil::~Coil(){}

/*_____  PUBLIC FUNCTIONS _____*/

void Coil::begin()
{
  motorCarriage.begin();
  motorWinding.begin();
}

void Coil::setWinding(float coilLength, float wireSize, unsigned long coilTurns)
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
  _stepsPerLayer = (M2_STEPS_PER_TR * _coilLength) / LEAD_SCREW_PITCH;

  // Determine when you need to start deceleration.
  // 1. Compute position at final speed, in step/us
  float Vf = (1 / (float)_minSpeed);
  // 2. Duration of acceleration, in micros seconds.
  unsigned long T = (_minSpeed - _maxSpeed) * _accDelay;
  // 3. Turns during acceleration.
  float stepsAcc = (Vf / 2) * T;
  // 4. Determine steps travel before start deceleration.
  _stepsTravel = _stepsPerLayer - stepsAcc;

#ifdef DEBUG
  Serial.print("Total steps : ");
  Serial.println(totalSteps(_coilTurns));
  delay(1000);
#endif
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


  bool direction = CLOCK;

  unsigned long layerStepsCounter = 0;
  _totalStepsCounter = 0;

  while(_totalStepsCounter < totalSteps(_coilTurns))
    {
      unsigned long currentMicros = micros();

      if(timer(currentMicros, &previousMicrosAcc, _accDelay))
	{
	  if(layerStepsCounter < _stepsTravel)
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

      if(layerStepsCounter > _stepsPerLayer)
	{
	  delayMotor_A = _minSpeed;
	  delayMotor_B = _minSpeed;
	  direction = !direction;
	  layerStepsCounter = 0;
	}
    }

#ifdef DEBUG
  Serial.print("step Tr : ");
  Serial.println(_totalStepsCounter);
#endif
}

// Not work, just travel carriage.
void Coil::oneLayer(float _coilLength, int _dir)
{
  unsigned long delayMotor_B = 600;
  unsigned long previousMicrosMotor_B = 0;

  unsigned long layerStepsCounter = 0;

  _stepsPerLayer = (M2_STEPS_PER_TR * _coilLength) / LEAD_SCREW_PITCH;

  while(layerStepsCounter < _stepsPerLayer)
    {
      unsigned long currentMicros = micros();
      if(timer(currentMicros, &previousMicrosMotor_B, delayMotor_B))
	{
	  motorCarriage.oneStep(_dir);
	  layerStepsCounter++;
	}
    }
}

//void Coil::move(bool carriage, bool coil)
//{
//  bool run = true;
//  bool direction = CLOCK;
//  int8_t value = 0;
//  unsigned long delayMotor_B = 600;
//  unsigned long previousMicrosMotor_B = 0;
//
//
//  _Encoder->setAccelerationEnabled(_Encoder->getAccelerationEnabled());
//
//  while(run)
//    {
//      ClickEncoder::Button buttonState = _Encoder->getButton();
//      if( buttonState == ClickEncoder::Clicked ){
//	  run=false;}
//
//      value += _Encoder->getValue();
//
//      Serial.print("Encoder value : ");
//      Serial.println(value);
//
//      value > 0 ? direction = CLOCK : direction = C_CLOCK;
//
//      while(value != 0)
//	{
//	  Serial.print("while value : ");
//	  Serial.println(value);
//	  unsigned long currentMicros = micros();
//	  if(timer(currentMicros, &previousMicrosMotor_B, delayMotor_B))
//	    {
//	      motorCarriage.oneStep(direction);
//	      value > 0 ? value-- : value++;
//	    }
//	}
//    }
//}







