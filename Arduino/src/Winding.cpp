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
static unsigned long ratioToDelay(float ratio, uint16_t delayMotor)
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
static void acceleration(bool acc, uint16_t *delayMotorA, uint16_t limitSpeed,
			 float ratio, uint16_t *delayMotorB)
{
  // if ratio is >= 1
  uint16_t *delayMotor_x = delayMotorA;
  uint16_t *delayMotor_y = delayMotorB;
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
static void acceleration(bool acc, uint16_t *delayMotor, uint16_t limitSpeed)
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
: _coilLength(0),
  _wireSize(0),
  _coilTurns(0),

  _windingSense(0),
  _carriageStartSense(0),

  _accDelay (400),
  _maxSpeed (30),
  _minSpeed (1400),
  _speed(0),

  _ratio(0),
  _direction(CLOCK),

  _stepsPerLayer(0),
  _stepsTravel(0),
  _totalStepsCounter(0),
  _layerStepsCounter(0)
{
  _Encoder = p_Encoder;
  _Display = p_Display;
  stepper.begin();
}


Coil::~Coil(){}

/*_____  PUBLIC FUNCTIONS _____*/
void Coil::setWinding(float coilLength, float wireSize, uint32_t coilTurns, bool windSense, bool startSense)
{
  _coilLength		= coilLength;
  _wireSize   		= wireSize;
  _coilTurns  		= coilTurns;
  _windingSense 	= windSense;
  _carriageStartSense 	= startSense;
}


void Coil::setSpeed(uint16_t accDelay, uint16_t maxSpeed, uint16_t minSpeed, uint16_t speed)
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

// Steps for motor who move carriage.
void Coil::computeStepPerLayer(float length)
{
  _stepsPerLayer = (M2_STEPS_PER_TR * length) / LEAD_SCREW_PITCH;

#ifdef DEBUGoff
  Serial.println("__________________");
  Serial.print("length : "); Serial.println(length);
  Serial.print("M2_STEPS_PER_TR : "); Serial.println(M2_STEPS_PER_TR);
  Serial.print("LEAD_SCREW_PITCH : "); Serial.println(LEAD_SCREW_PITCH);
  Serial.print("_stepsPerLayer : "); Serial.println(_stepsPerLayer);
  Serial.println("__________________");
#endif
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
  _stepsTravel = totalSteps - (uint32_t)stepsAcc;
}
void Coil::computeAll()
{
  computeRatio();
  computeStepsTravel(_stepsPerLayer);

#ifdef DEBUGoff
  Serial.print("MaxSpeed : "); Serial.println(_maxSpeed);
  Serial.print("MinSpeed : "); Serial.println(_minSpeed);
  Serial.print("AccDelay : "); Serial.println(_accDelay);
  Serial.print("_ratio : "); Serial.println(_ratio);
  Serial.print("_stepPerLayer : "); Serial.println(_stepsPerLayer);
  Serial.print("turnPerLayer : "); Serial.println(StepsToTurns(_stepsPerLayer * _ratio));
  Serial.print("_stepsTravel : "); Serial.println(_stepsTravel);
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

  // Little delay to mark the end, and back to the start position.
  delay(800);
  runOnlyCarriage(dir, dist);
}


bool Coil::runMultiLayer(bool isNewCoil)
{
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
  uint16_t delayMotorWinding = _minSpeed;
  uint16_t delayMotorCarriage = (_minSpeed * _ratio);
  uint32_t lastMicrosMotorWinding = 0;
  uint32_t lastMicrosMotorCarriage = 0;
  uint32_t lastMicrosAcc = 0;

  // Control to give the exact number of steps on coil and on carriage.
  uint32_t coilLayerSteps = (_stepsPerLayer * _ratio);
  uint32_t counterCoilLayerSteps = 0;


  while((_totalStepsCounter < TurnToSteps(_coilTurns)) &&
      ((_layerStepsCounter < _stepsPerLayer) || (counterCoilLayerSteps < coilLayerSteps)))
    {
      // If user click on encoder, isSuspend become true and break the loop.
      if( suspend() == true) return true;
      else
	{
	  uint32_t currentMicros = micros();

	  if(timer(currentMicros, &lastMicrosAcc, _accDelay))
	    {
	      if((_layerStepsCounter >= _stepsTravel) ||
		  (_totalStepsCounter >= TurnToSteps((_coilTurns-2)))) // -2Tr is to give deceleration when winding is finished
		{
		  // Deceleration
		  acceleration(false, &delayMotorWinding, _minSpeed, _ratio, &delayMotorCarriage);
		}
	      else
		{
		  // Acceleration
		  acceleration(true, &delayMotorWinding, _speed, _ratio, &delayMotorCarriage);
		}
	    }

	  if(timer(currentMicros, &lastMicrosMotorWinding, delayMotorWinding)&&
	      (counterCoilLayerSteps < coilLayerSteps))
	    {
	      stepper.coil_oneStep(C_CLOCK);
	      _totalStepsCounter ++;
	      counterCoilLayerSteps++;
	    }

	  if(timer(currentMicros, &lastMicrosMotorCarriage, delayMotorCarriage)&&
	      (_layerStepsCounter < _stepsPerLayer))
	    {
	      stepper.carriage_oneStep(_direction);
	      _layerStepsCounter ++;
	    }
	}
    }

  return false;
}


bool Coil::runOnlyCarriage(bool dir, float distance)
{
  // Number of steps for this distance, set "_stepsPerLayer" !
  computeStepPerLayer(distance);
  // Set "_stepsTravel" for start deceleration.
  computeStepsTravel(_stepsPerLayer);

  uint16_t delayMotor = _minSpeed;
  uint32_t lastMicrosMotor = 0;
  uint32_t lastMicrosAcc = 0;

  uint32_t stepsCounter = 0;

  while(stepsCounter < _stepsPerLayer )
    {
      // If user click on encoder, isSuspend become true and break the loop.
      if( suspend() == true) return true;
      else
	{
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
	      stepper.carriage_oneStep(dir);
	      stepsCounter ++;
	    }
	}
    }
  return false;
}


bool Coil::runOnlyCoil(bool dir, uint32_t turns)
{
  // Set "_stepsTravel" for start deceleration.
  computeStepsTravel(TurnToSteps(turns));

  uint16_t delayMotor = _minSpeed;
  uint32_t lastMicrosMotor = 0;
  uint32_t lastMicrosAcc = 0;

  uint32_t stepsCounter = 0;
  bool isStop = false;

  while( !isStop && stepsCounter < TurnToSteps(turns))
    {
      // If user click on encoder, isSuspend become true and break the loop.
      if( suspend() == true) return true;
      else
	{
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
	      stepper.coil_oneStep(!dir);
	      stepsCounter ++;
	    }
	}
    }
  return false;
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
  stepper.coil_disable();
  stepper.carriage_disable();
}


uint32_t Coil::getTurns()
{
  return _coilTurns;
}


uint32_t Coil::getCurrentTurns()
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
