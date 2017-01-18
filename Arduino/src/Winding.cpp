#include "Winding.h"

/*_____ PRIVATE FONCTIONS _____ */

/******************************************************************************
 * brief   : Convert number of turns in steps.
 * details : Convert number of turns in steps from steps motor for winding.
 * _coilTurns, number of turns that must be winding.
 * return  : Return steps.
 ******************************************************************************/
#define TurnToSteps(coilTurns) coilTurns * M1_STEPS_PER_TR

/******************************************************************************
 * brief   : Convert reduction ratio into delay.
 * details : If the ratio is inferior to 1 we need to convert delay
 * to keep the good reduction ratio between motors in acceleration() function.
 * ratio, value of reduction "_ratio".
 * delayMotor, delay on the motor who is the reference.
 * return  : delay value.
 ******************************************************************************/
static unsigned long ratioToDelay(float ratio, unsigned long delayMotor)
{
  unsigned long result;

  ratio >= 1 ? (result = ratio * delayMotor) : (result = delayMotor / ratio);

  return result;
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
	  *delayMotor_x -= 1;
	  *delayMotor_y = ratioToDelay(ratio, (*delayMotor_x));
	}
    }
  // Deceleration.
  else
    {
      if(*delayMotor_x < limitSpeed )
	{
	  *delayMotor_x += 1;
	  *delayMotor_y = ratioToDelay(ratio, (*delayMotor_x));
	}
    }
}

// For one motor only.
static void acceleration(bool acc, unsigned long *delayMotor, unsigned long limitSpeed)
{
  //Acceleration.
  if(acc && (*delayMotor > limitSpeed))
    {
      *delayMotor -= 1;
    }
  // Deceleration.
  else
    {
      if(*delayMotor < limitSpeed )
	{
	  *delayMotor += 1;
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
{
  motorCarriage.begin();
  motorWinding.begin();
}


Coil::~Coil(){}

/*_____  PUBLIC FUNCTIONS _____*/
void Coil::setWinding(float coilLength, float wireSize, unsigned long coilTurns)
{
  _coilLength = coilLength;
  _wireSize   = wireSize;
  _coilTurns  = coilTurns;
}


void Coil::setSpeed(unsigned long accDelay, unsigned long maxSpeed, unsigned long minSpeed)
{
  _accDelay = accDelay;
  _maxSpeed = maxSpeed;
  _minSpeed = minSpeed;
}


void Coil::computeStepPerLayer(float length)
{
  _stepsPerLayer = (M2_STEPS_PER_TR * length) / LEAD_SCREW_PITCH;
}


void Coil::computeRatio()
{
  // Number of steps for "carriage motor" advance, depending wire size and lead screw.
  float pitchToSteps = (M2_STEPS_PER_TR * _wireSize) / LEAD_SCREW_PITCH;
  // Reduction ratio due, between motors.
  _ratio = M1_STEPS_PER_TR / pitchToSteps;
  // Steps for winding one layer.
  computeStepPerLayer(_coilLength);

#ifdef DEBUGOFF
  Serial.print("pitchToStep : ");
  Serial.println(pitchToSteps);
#endif
}


// Determine when you need to start deceleration.
void Coil::computeStepsTravel(float totalSteps)
{
  // 1. Duration of acceleration, in micros seconds.
  float T = (_minSpeed - _maxSpeed) * _accDelay;
  // 2. Number of steps during acceleration.
  float stepsAcc = ((0.5 * (1.0/_accDelay) * (T*T)));
  // Convert into seconde.
  stepsAcc /= 1000000;
  // Add initial steps.
  stepsAcc += (T/_minSpeed);

  // 3. Determine steps travel before start deceleration.
  _stepsTravel = totalSteps - (unsigned long)stepsAcc;

#ifdef DEBUGOFF
  Serial.print("T : ");
  Serial.println(T);
  Serial.print("stepsAcc : ");
  Serial.println(stepsAcc);
#endif
}
void Coil::computeAll()
{
  computeRatio();
  computeStepsTravel(_stepsPerLayer);

#ifdef DEBUGOFF
  Serial.print("MaxSpeed : ");
  Serial.println(_maxSpeed);
  Serial.print("MinSpeed : ");
  Serial.println(_minSpeed);
  Serial.print("AccDelay : ");
  Serial.println(_accDelay);
  Serial.print("_ratio : ");
  Serial.println(_ratio);
  Serial.print("_stepPerLayer : ");
  Serial.println(_stepsPerLayer);
  Serial.print("_stepsTravel : ");
  Serial.println(_stepsTravel);
  delay(1000);
#endif
}

void Coil::homing(bool dir, unsigned long layerStepsCounter)
{
  // Depending on the direction of travel, we need to re-invert and recalculate displacement.
  // Else we use layerStepsCounter and pre-reversed direction.
  float dist = layerStepsCounter;

  if(dir)
    {
      (dist = _stepsPerLayer - layerStepsCounter);
      dir = !dir;
    }
  // Convert step into distance.
  dist = (dist*LEAD_SCREW_PITCH) / M2_STEPS_PER_TR;

  // Little delay to mark end, and back to the start position.
  delay(500);
  runOnlyCarriage(dir, dist);
}


void Coil::runMultiLayer()
{
  // Compute all values to make winding.
  computeAll();

  bool direction = CLOCK;
  unsigned long totalStepsCounter = 0;
  unsigned long layerStepsCounter = 0;

  while(totalStepsCounter < TurnToSteps(_coilTurns))
    {
      layerStepsCounter = 0;

      runOneLayer(direction, &totalStepsCounter, &layerStepsCounter);

      // Invert direction when one layer is finished
      direction = !direction;
    }

  // Direction is already inverted !
  homing(direction, layerStepsCounter);
}


void Coil::runOneLayer(bool dir, unsigned long *p_totalStepsCounter, unsigned long *p_layerStepsCounter)
{
  unsigned long delayMotorWinding = _minSpeed;
  unsigned long delayMotorCarriage = _minSpeed;
  unsigned long lastMicrosMotorWinding = 0;
  unsigned long lastMicrosMotorCarriage = 0;
  unsigned long lastMicrosAcc = 0;

  while((*p_totalStepsCounter< TurnToSteps(_coilTurns)) && (*p_layerStepsCounter < _stepsPerLayer))
    {
      unsigned long currentMicros = micros();

      if(timer(currentMicros, &lastMicrosAcc, _accDelay))
	{
	  if(*p_layerStepsCounter < _stepsTravel)
	    {
	      // Acceleration
	      acceleration(true, &delayMotorWinding, _maxSpeed, _ratio, &delayMotorCarriage);
	    }
	  else
	    {
	      // Deceleration
	      acceleration(false, &delayMotorWinding, _minSpeed, _ratio, &delayMotorCarriage);
	    }
	}
      if(timer(currentMicros, &lastMicrosMotorWinding, delayMotorWinding))
	{
	  motorWinding.oneStep(C_CLOCK);
	  *p_totalStepsCounter += 1;
	}
      if(timer(currentMicros, &lastMicrosMotorCarriage, delayMotorCarriage))
	{
	  motorCarriage.oneStep(dir);
	  *p_layerStepsCounter += 1;
	}
    }
  //  Serial.println("total steps"), Serial.println(layerStepsCounter);
}


void Coil::runOnlyCarriage(bool dir, float distance)
{
  // Number of steps for this distance, set "_stepsPerLayer" !
  computeStepPerLayer(distance);
  // Set "_stepsTravel" for start deceleration.
  computeStepsTravel(_stepsPerLayer);

  unsigned long delayMotor = _minSpeed;
  unsigned long lastMicrosMotor = 0;
  unsigned long lastMicrosAcc = 0;

  unsigned long stepsCounter = 0;

  while( stepsCounter < _stepsPerLayer )
    {
      unsigned long currentMicros = micros();

      if(timer(currentMicros, &lastMicrosAcc, _accDelay))
	{
	  if(stepsCounter < _stepsTravel)
	    {
	      // Acceleration
	      acceleration(ACCELERATION, &delayMotor, _maxSpeed);
	    }
	  else
	    {
	      // Deceleration
	      acceleration(DECELERATION, &delayMotor, _minSpeed);
	    }
	}

      if(timer(currentMicros, &lastMicrosMotor, delayMotor))
	{
	  motorCarriage.oneStep(dir);
	  stepsCounter += 1;
	}
    }
}


void Coil::runOnlyCoil(bool dir, float turns)
{
  // Set "_stepsTravel" for start deceleration.
  computeStepsTravel(TurnToSteps(turns));

  unsigned long delayMotor = _minSpeed;
  unsigned long lastMicrosMotor = 0;
  unsigned long lastMicrosAcc = 0;

  unsigned long stepsCounter = 0;

  while( stepsCounter < TurnToSteps(turns))
    {
      unsigned long currentMicros = micros();

      if(timer(currentMicros, &lastMicrosAcc, _accDelay))
	{
	  if(stepsCounter < _stepsTravel)
	    {
	      // Acceleration
	      acceleration(ACCELERATION, &delayMotor, _maxSpeed);
	    }
	  else
	    {
	      // Deceleration
	      acceleration(DECELERATION, &delayMotor, _minSpeed);
	    }
	}

      if(timer(currentMicros, &lastMicrosMotor, delayMotor))
	{
	  motorWinding.oneStep(!dir);
	  stepsCounter += 1;
	}
    }
}

void Coil::stopMotion()
{

}

void Coil::disableMotors()
{
  motorWinding.disable();
  motorCarriage.disable();
}
