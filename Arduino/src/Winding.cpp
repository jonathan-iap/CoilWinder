#include "Winding.h"

/*_____ PRIVATE FONCTIONS _____ */

/******************************************************************************
 * brief   : Convert number of turns in steps.
 * details : Convert number of turns in steps from steps motor for winding.
 * _coilTurns, number of turns that must be winding.
 * return  : Return steps.
 ******************************************************************************/
#define TurnToSteps(coilTurns) coilTurns * STEPS_PER_TR
#define StepsToTurns(coilSteps) coilSteps / STEPS_PER_TR


/*_____ CONSTRUCTOR _____ */

Coil::Coil(ClickEncoder *p_Encoder, Display *p_Display)
: _coilLength(0),
  _wireSize(0),
  _coilTurns(0),

  _windingSense(0),
  _carriageStartSense(0),

  _accIncr(0),
  _accDelay(0),
  _maxSpeed(0),
  _minSpeed(0),
  _speed(0),
  _speedPercent(0)
{
  _Encoder = p_Encoder;
  _Display = p_Display;
}


Coil::~Coil(){}

/*_____  PUBLIC FUNCTIONS _____*/
void Coil::setWinding(float coilLength, float wireSize, uint32_t coilTurns, bool windSense, bool carSense)
{
  _coilLength		= coilLength;
  _wireSize   		= wireSize;
  _coilTurns  		= coilTurns;
  _windingSense		= windSense;
  _carriageStartSense	= carSense;
}


void Coil::setSpeed(uint16_t accIncr, uint16_t accDelay, uint16_t maxSpeed, uint16_t minSpeed, uint16_t speed, int8_t speedPercent)
{
  _accIncr  = accIncr;
  _accDelay = accDelay;
  _maxSpeed = maxSpeed;
  _minSpeed = minSpeed;
  _speed    = speed;
  _speedPercent = speedPercent;
}


// Allow to update speed during winding or displacement
void Coil::updateSpeed(int8_t *oldPercent, uint16_t *speedSet)
{
  _speedPercent += (_Encoder->getValue()*5);

  if(*oldPercent != _speedPercent)
    {
      *oldPercent = _speedPercent;

      _speedPercent = constrain(_speedPercent, 1, 100);

      _speed = map(_speedPercent, 0, 100, _minSpeed, _maxSpeed);

      M_setSpeed(_speed);

      _Display->windingGetSpeedPercent(_speedPercent);

      *speedSet =  _speed;
    }
  else{};
}

bool Coil::computeWinding(float coilLength, float wireSize, uint16_t *nbTrForOneLayer, uint16_t *stepsPerTr )
{
  float tmp_nbTrForOneLayer = 0;
  float tmp_step = 0;

  tmp_nbTrForOneLayer = coilLength / wireSize;
  *nbTrForOneLayer = (uint16_t)tmp_nbTrForOneLayer;

  tmp_step = (wireSize * (float)STEPS_PER_TR) / (float)LEAD_SCREW_PITCH;
  *stepsPerTr = (uint16_t)tmp_step;

  if(tmp_nbTrForOneLayer < STEPS_PER_TR) return true;
  else return false;
}

/******************************************************************************
 * brief   : Travel computing
 * details : Decompose the distance in steps per pass.
 * Allows to use more large and flexible value of displacement.
 * Values are directly returns by pointers
 ******************************************************************************/
void Coil::computeTravel(float distance, uint16_t *nbPass, uint16_t *stepsPerTr)
{
  float tmp_newDistance = distance;
  float tmp_nbPass = 0.00;
  float tmp_stepsPerTr = 0.00;
  float ratio = 0.00;

  tmp_nbPass = 1;

  do
    {
      // If number of steps will exceed the max value of uint16_t
      // ratio will be superior to 1 and a pass will be added
      ratio = (((tmp_newDistance / (float)LEAD_SCREW_PITCH) * (float)STEPS_PER_TR) / (float)MAX_INTEGER);

      if(ratio > 1.00)
	{
	  tmp_nbPass += 1;
	  tmp_newDistance = distance / tmp_nbPass ;
	}
      else
	{
	  tmp_stepsPerTr = ratio * (float)MAX_INTEGER;
	}
    }
  while(ratio > 1.00);

  *nbPass = (uint16_t)tmp_nbPass;
  *stepsPerTr = (uint16_t)tmp_stepsPerTr;
}




void Coil::acceleration(uint16_t speedSet, uint16_t *accSpeed, uint32_t *oldTime)
{
  uint32_t currentMicros = micros();

  if(timer(currentMicros, oldTime, _accDelay) && *accSpeed < speedSet)
    {
      *accSpeed += _accIncr;
      M_setSpeed( *accSpeed);
    }
}




float Coil::getRelativeHome(float homePosition, bool dir)
{
  float tmp_value = M_getDisplacement();

  if(!dir) tmp_value*=-1;

  return tmp_value += homePosition;
}





void Coil::winding(bool isNewCoil)
{
  bool run = true;

  uint16_t nbPass = 0;
  uint16_t steps = 0;
  bool isCoilFastest = 0;

  uint16_t speedSet = _speed;
  int8_t old_speedPercent = _speedPercent;
  uint32_t lastMicrosAcc = 0;

  _speed = _minSpeed;

  isCoilFastest = computeWinding(_coilLength, _wireSize, &nbPass, &steps);

  M_setWindingDisplacement(nbPass, steps, _coilTurns, STEPS_PER_TR, isCoilFastest);

  M_setMotors(COIL, _windingSense, CARRIAGE, _carriageStartSense, _minSpeed);

  M_start();

  while( !M_getWindingStatus() && run)
    {
      if( suspend() == true)
	{
	  run = false;
	  M_stop();
	}
      else
	{
	  acceleration(speedSet, &_speed, &lastMicrosAcc);
	  updateSpeed(&old_speedPercent, &speedSet);
	  _Display->windingGetTurns(_coilTurns, M_getCoilTr());
	}
    }
}




void Coil::runOnlyCarriage(bool dir, float distance, float *homingPosition)
{
  bool run = true;

  uint16_t nbPass = 0;
  uint16_t steps = 0;

  uint16_t speedSet = _speed;
  int8_t old_speedPercent = _speedPercent;
  uint32_t lastMicrosAcc = 0;

  // Acceleration need to have current speed set to minimum.
  _speed = _minSpeed;

  _Display->clear();
  _Display->windingGetSpeedPercent(_speedPercent);
  _Display->windingGetDisplacement(distance, M_getDisplacement());

  computeTravel(distance, &nbPass, &steps);
  M_setSimpleDisplacement(TRAVELING, nbPass, steps);

  M_setMotors(0, 0, CARRIAGE, dir, _minSpeed);

  M_start();

  while( !M_getWindingStatus() && run)
    {
      if( suspend() == true)
	{
	  run = false;
	  M_stop();
	}
      else
	{
	  acceleration(speedSet, &_speed, &lastMicrosAcc);
	  updateSpeed(&old_speedPercent, &speedSet);

	  _Display->windingGetDisplacement(distance, M_getDisplacement());
	}
    }

  *homingPosition = getRelativeHome(*homingPosition, dir);
}




void Coil::runOnlyCoil(bool dir, uint16_t turns)
{
  bool run = true;

  uint16_t speedSet = _speed;
  int8_t old_speedPercent = _speedPercent;
  uint32_t lastMicrosAcc = 0;

  _speed = _minSpeed;

  _Display->clear();
  _Display->windingGetSpeedPercent(_speedPercent);
  _Display->windingGetTurns(turns, M_getCoilTr());

  M_setSimpleDisplacement(ROTATION, turns, STEPS_PER_TR);

  M_setMotors(COIL, dir, 0, 0, _minSpeed);

  M_start();

  while( !M_getWindingStatus() && run )
    {
      if( suspend() == true)
	{
	  run = false;
	  M_stop();
	}
      else
	{
	  acceleration(speedSet, &_speed, &lastMicrosAcc);
	  updateSpeed(&old_speedPercent, &speedSet);

	  _Display->windingGetTurns(turns, M_getCoilTr());
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


//void Coil::disableMotors()
//{
//stepper.coil_disable();
//stepper.carriage_disable();
//}


//uint32_t Coil::getTurns()
//{
//  return _coilTurns;
//}
//
//
//uint32_t Coil::getCurrentTurns()
//{
//  return StepsToTurns(_totalStepsCounter);
//}
//
//
//uint32_t Coil::getTotalStepsCounter()
//{
//  return _totalStepsCounter;
//}
//
//
//uint32_t Coil::getLayerStepsCounter()
//{
//  return _layerStepsCounter;
//}
//
//
//uint32_t Coil::getLayerCoilStepsCounter()
//{
//  return _layerCoilStepsCounter;
//}
