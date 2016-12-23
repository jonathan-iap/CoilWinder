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
static unsigned long ratioToDelay(double ratio, unsigned long delayMotor)
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
			 double ratio, unsigned long *delayMotorB)
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
  _stepsTravel(0)
{}

Coil::~Coil(){}

/*_____  PUBLIC FUNCTIONS _____*/

void Coil::begin()
{
  motorCarriage.begin();
  motorWinding.begin();
}

void Coil::getWinding(double coilLength, double wireSize, unsigned long coilTurns)
{
  _coilLength = coilLength;
  _wireSize   = wireSize;
  _coilTurns = coilTurns;
}

void Coil::getSpeed(unsigned long accDelay, unsigned long maxSpeed, unsigned long minSpeed)
{
  _accDelay = accDelay;
  _maxSpeed = maxSpeed;
  _minSpeed = minSpeed;
}

void Coil::computing()
{
  /*____Calculation____*/
  // Number of steps for carriage motor advance, when coil (winding motor) make one turn.
  double pitchToSteps = (M2_STEPS_PER_TR * _wireSize) / LEAD_SCREW_PITCH;
  // Reduction ratio due, between motors.
  _ratio = M1_STEPS_PER_TR / pitchToSteps;
  // Steps for winding one layer.
  _stepsPerLayer = (M2_STEPS_PER_TR * _coilLength) / LEAD_SCREW_PITCH;

  // Determine when you need to start deceleration.
  // 1. Compute position at final speed, in step/us
  double Vf = (1 / (double)_minSpeed);
  // 2. Duration of acceleration, in micros seconds.
  unsigned long T = (_minSpeed - _maxSpeed) * _accDelay;
  // 3. Turns during acceleration.
  double stepsAcc = (Vf / 2) * T;
  // 4. Determine steps travel before start deceleration.
  _stepsTravel = _stepsPerLayer - (unsigned long)stepsAcc;

#ifdef DEBUG
  Serial.print("pitchToStep : ");
  Serial.println(pitchToSteps);
  Serial.print("_ratio : ");
  Serial.println(_ratio);
  Serial.print("_stepPerLayer : ");
  Serial.println(_stepsPerLayer);
  Serial.print("Vf : ");
  Serial.println(Vf);
  Serial.print("T : ");
  Serial.println(T);
  Serial.print("stepsAcc : ");
  Serial.println(stepsAcc);
  Serial.print("_stepsTravel : ");
  Serial.println(_stepsTravel);
  delay(1000);
#endif
}


double Coil::getValue()
{
  return _stepsPerLayer;
}

void Coil::run()
{
  // Compute all values to make winding.
  computing();

  //  unsigned long delayMotor_A = _minSpeed;
  //  unsigned long delayMotor_B = _minSpeed;
  //  unsigned long previousMicrosMotor_A = 0;
  //  unsigned long previousMicrosMotor_B = 0;
  //  unsigned long previousMicrosAcc = 0;
  //
  bool direction = CLOCK;
  //
  //  unsigned long layerStepsCounter = 0;
  unsigned long totalStepsCounter = 0;

  while(totalStepsCounter < totalSteps(_coilTurns))
    {
      //      unsigned long currentMicros = micros();
      //
      //      if(timer(currentMicros, &previousMicrosAcc, _accDelay))
      //	{
      //	  if(layerStepsCounter < _stepsTravel)
      //	    {
      //	      // Acceleration
      //	      acceleration(true, &delayMotor_A, _maxSpeed, _ratio, &delayMotor_B);
      //	    }
      //	  else
      //	    {
      //	      // Deceleration
      //	      acceleration(false, &delayMotor_A, _minSpeed, _ratio, &delayMotor_B);
      //	    }
      //	}
      //      if(timer(currentMicros, &previousMicrosMotor_A, delayMotor_A))
      //	{
      //	  motorWinding.oneStep(CLOCK);
      //	  _totalStepsCounter++;
      //	}
      //      if(timer(currentMicros, &previousMicrosMotor_B, delayMotor_B))
      //	{
      //	  motorCarriage.oneStep(direction);
      //	  layerStepsCounter++;
      //	}
      oneLayer(direction, true, true, &totalStepsCounter);

      direction = !direction;
    }

#ifdef DEBUG
  Serial.print("step Tr : ");
  Serial.println(totalStepsCounter);
#endif
}

void Coil::oneLayer(bool dir, bool M_carriage, bool M_winding, unsigned long *p_totalStepsCounter)
{
  unsigned long delayMotorWinding = _minSpeed;
  unsigned long delayMotorCarriage = _minSpeed;
  unsigned long lastMicrosMotorWinding = 0;
  unsigned long lastMicrosMotorCarriage = 0;
  unsigned long lastMicrosAcc = 0;

  unsigned long layerStepsCounter = 0;

  bool debug = true;

  Serial.println("------------------");
  Serial.print("stepsPerLayer : ");
  Serial.println(_stepsPerLayer );
  Serial.println("------------------");
  delay(1000);

  while(layerStepsCounter < _stepsPerLayer)// && *p_totalStepsCounter < totalSteps(_coilTurns))
    {
      unsigned long currentMicros = micros();

      if(timer(currentMicros, &lastMicrosAcc, _accDelay))
	{
	  if(layerStepsCounter < _stepsTravel)
	    {
	      // Acceleration
	      acceleration(true, &delayMotorWinding, _maxSpeed, _ratio, &delayMotorCarriage);
	      if(delayMotorCarriage == _maxSpeed && debug)
		{
		  debug = false;
		  Serial.print("acceleration step : "), Serial.println(layerStepsCounter);
		}
	    }
	  else
	    {
	      if(delayMotorCarriage == _maxSpeed && !debug)
		{
		  debug = true;
		  Serial.print("decceleration step : "), Serial.println(layerStepsCounter);
		}
	      // Deceleration
	      acceleration(false, &delayMotorWinding, _minSpeed, _ratio, &delayMotorCarriage);
	    }
	}
      if(M_winding && timer(currentMicros, &lastMicrosMotorWinding, delayMotorWinding))
	{
	  motorWinding.oneStep(CLOCK);
	  *p_totalStepsCounter += 1;
	}
      if(M_carriage && timer(currentMicros, &lastMicrosMotorCarriage, delayMotorCarriage))
	{
	  motorCarriage.oneStep(dir);
	  layerStepsCounter++;
	}
    }
  Serial.println(layerStepsCounter);
}

void Coil::disableMotors()
{
  motorWinding.disable();
  motorCarriage.disable();
}
