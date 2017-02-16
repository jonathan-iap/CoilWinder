#include "Winding.h"

/*_____ PRIVATE FONCTIONS _____ */

/******************************************************************************
 * brief   : Convert number of turns in steps.
 * details : Convert number of turns in steps from steps motor for winding.
 * _coilTurns, number of turns that must be winding.
 * return  : Return steps.
 ******************************************************************************/
#define TurnToSteps(coilTurns) coilTurns * M1_STEPS_PER_TR
#define StepsToTurns(coilSteps) coilSteps / M1_STEPS_PER_TR

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

Coil::Coil(ClickEncoder *p_Encoder, Display *p_Display)
: motorWinding (M1_DIR, M1_STEP, M1_EN, M1_STEPS_PER_TR),
  motorCarriage (M2_DIR, M2_STEP, M2_EN, M2_STEPS_PER_TR),

  _coilLength(0),
  _wireSize(0),
  _coilTurns(0),

  _accDelay (400),
  _maxSpeed (30),
  _minSpeed (1400),
  _speed(0),

  _ratio(0),
  _stepsPerLayer(0),
  _stepsTravel(0),
  _totalStepsCounter(0),
  _layerStepsCounter(0),

  _direction(CLOCK)
{
  _Encoder = p_Encoder;
  _Display = p_Display;
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


void Coil::setSpeed(unsigned long accDelay, unsigned long maxSpeed, unsigned long minSpeed, uint16_t speed)
{
  _accDelay = accDelay;
  _maxSpeed = maxSpeed;
  _minSpeed = minSpeed;
  _speed = speed;
}


void Coil::setSteps(uint32_t totalSteps, uint32_t layerSteps)
{
  _totalStepsCounter = totalSteps;
  _layerStepsCounter = layerSteps;
}


void Coil::updateSpeed(uint16_t speed)
{
  _speed = speed;
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

void Coil::homing(bool dir)
{
  // Depending the direction of travel, we need to invert and recalculate displacement.
  // Else we use "layerStepsCounter" and "direction".
  float dist = _layerStepsCounter;

  if(dir) dir = !dir;
  else (dist = _stepsPerLayer - _layerStepsCounter);
  // Convert step into distance.
  dist = (dist*LEAD_SCREW_PITCH) / M2_STEPS_PER_TR;

  // Little delay to mark end, and back to the start position.
  delay(800);
  runOnlyCarriage(dir, dist);
}


bool Coil::runMultiLayer(bool isNewCoil)
{
  Serial.println("Run pass ");

  bool isResume = false;
  bool isbackHome = true;
  bool isSuspend = false;

  // Compute all values to make winding.
  computeAll();

  // Resume winding
  if( !isNewCoil )
    {
      isResume = true;
      // Protection if user click on and winding is finish. Carriage don't move.
      if(_totalStepsCounter >= TurnToSteps(_coilTurns)){
	  isbackHome = false;
      }
    }
  // New winding
  else
    {
      _direction = C_CLOCK; // To start left to right.
      _totalStepsCounter = 0;
      _layerStepsCounter = 0;
    }

  // Display value that are used for current winding.
  _Display->engineWindingValue(_coilLength, _wireSize, _coilTurns, getCurrentTurns());

  while(_totalStepsCounter < TurnToSteps(_coilTurns) && !isSuspend)
    {
      // For resume winding, we need must finish the layer first.
      if(isResume)
	{
	  isResume = false;
	}
      // For each new layer (and after the finished layer of a resume session)
      else
	{
	  // initialize for the next loop.
	  _layerStepsCounter = 0;
	  // Invert direction when one layer is finished.
	  _direction = !_direction;
	}
      // winding one layer.
      isSuspend = runOneLayer();

      // refresh turn by layer on lcd.
      _Display->windingTurns(_coilTurns, StepsToTurns(_totalStepsCounter));
    }

  if( !isSuspend && isbackHome)
    {
      // Return to the first position, only if winding is finished.
      homing(_direction);
    }

  // Become false if we interrupt winding
  return isSuspend;
}


bool Coil::runOneLayer()
{
  unsigned long delayMotorWinding = _minSpeed;
  unsigned long delayMotorCarriage = _minSpeed;
  unsigned long lastMicrosMotorWinding = 0;
  unsigned long lastMicrosMotorCarriage = 0;
  unsigned long lastMicrosAcc = 0;

  while((_totalStepsCounter < TurnToSteps(_coilTurns)) &&
      (_layerStepsCounter < _stepsPerLayer))
    {

      // If user click on encoder, isSuspend become true and break the loop.
      if( suspend() == true) return true;

      unsigned long currentMicros = micros();

      if(timer(currentMicros, &lastMicrosAcc, _accDelay))
	{
	  if(_layerStepsCounter < _stepsTravel)
	    {
	      // Acceleration
	      acceleration(true, &delayMotorWinding, _speed, _ratio, &delayMotorCarriage);
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
	  _totalStepsCounter += 1;
	}
      if(timer(currentMicros, &lastMicrosMotorCarriage, delayMotorCarriage))
	{
	  motorCarriage.oneStep(_direction);
	  _layerStepsCounter += 1;
	}
    }

  return false;
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
  bool isStop = false;

  while( !isStop && stepsCounter < _stepsPerLayer )
    {
      isStop = suspend();

      unsigned long currentMicros = micros();

      if(timer(currentMicros, &lastMicrosAcc, _accDelay))
	{
	  if(stepsCounter < _stepsTravel)
	    {
	      // Acceleration
	      acceleration(ACCELERATION, &delayMotor, _speed);
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
	  stepsCounter ++;
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
  bool isStop = false;

  while( !isStop && stepsCounter < TurnToSteps(turns))
    {
      isStop = suspend();
      unsigned long currentMicros = micros();

      if(timer(currentMicros, &lastMicrosAcc, _accDelay))
	{
	  if(stepsCounter < _stepsTravel)
	    {
	      // Acceleration
	      acceleration(ACCELERATION, &delayMotor, _speed);
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
	  stepsCounter ++;
	}
    }
}

// Check if button was pressed
bool Coil::suspend()
{
  if( _Encoder->getButton() == ClickEncoder::Clicked )
    {
      return true;
    }
  else return false;
}


void Coil::disableMotors()
{
  motorWinding.disable();
  motorCarriage.disable();
}


uint16_t Coil::getTurns()
{
  return _coilTurns;
}


uint16_t Coil::getCurrentTurns()
{
  return StepsToTurns(_totalStepsCounter);
}


uint32_t Coil::getTotalStepsCounter()
{
  return _totalStepsCounter;
}


uint32_t Coil::getLayerStepsCounter()
{
  return _layerStepsCounter;
}
