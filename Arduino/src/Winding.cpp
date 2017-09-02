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
  _speedPercent(0),

  _saveCarrPass(0),
  _saveCarrStepPerPass(0),
  _saveCarrDir(0),
  _saveCoilTr(0),
  _saveCoilStepPerTr(0),
  _saveCoilDir(0)
{
  _Encoder = p_Encoder;
  _Display = p_Display;
}


Coil::~Coil(){}

/*_____  PUBLIC FUNCTIONS _____*/
void Coil::setWinding(float coilLength, float wireSize, uint16_t coilTurns, bool windSense, bool carSense)
{
  _coilLength		= coilLength;
  _wireSize   		= wireSize;
  _coilTurns  		= coilTurns;
  _windingSense		= windSense;
  _carriageStartSense	= carSense;

  //  Serial.println(" ");
  //  Serial.println("****** SetWinding ******");
  //  Serial.print("_coilLength : "), Serial.println(_coilLength);
  //  Serial.print("_wireSize : "), Serial.println(_wireSize);
  //  Serial.print("_coilTurns : "), Serial.println(_coilTurns);
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





void Coil::setSteps(uint16_t carrPass, uint16_t carrStepPerPass, bool carrDir,
		    uint16_t coilTr, uint16_t coilStepPerTr, bool coilDir)
{
  _saveCarrPass        = carrPass;
  _saveCarrStepPerPass = carrStepPerPass;
  _saveCarrDir         = carrDir;

  _saveCoilTr          = coilTr;
  _saveCoilStepPerTr   = coilStepPerTr;
  _saveCoilDir         = coilDir;
}




// Allow to update speed during winding or displacement
bool Coil::updateSpeed(int8_t *oldPercent, uint16_t *speedSet, uint8_t offset)
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
      return true;
    }
  else return false;
}



/******************************************************************************
 * brief   : Winding computing
 * details : Compute how many steps by turn we need to make one layer
 * Values are directly returns by pointers
 * If wire size if greater than lead screw pitch, we need to change how to
 * count steps in engine "M_engine()".
 * Because the carriage will reach the maximum speed before the coil.
 ******************************************************************************/
bool Coil::computeWinding(float coilLength, float wireSize, uint16_t *nbTrForOneLayer, uint16_t *stepsPerTr )
{
  float tmp_nbTrForOneLayer = 0;
  float tmp_step = 0;

  tmp_nbTrForOneLayer = coilLength / wireSize;
  *nbTrForOneLayer = (uint16_t)tmp_nbTrForOneLayer;

  tmp_step = (wireSize * (float)STEPS_PER_TR) / (float)LEAD_SCREW_PITCH;
  *stepsPerTr = (uint16_t)tmp_step;

  if(wireSize < LEAD_SCREW_PITCH) return true;
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




void Coil::refreshDisplay(bool *run, uint32_t *oldTime)
{
  uint32_t currentMicros = millis();

  if(timer(currentMicros, oldTime, 1000) && *run)
    {
      *run = false;
      _Display->engineWindingRefresh(_coilLength, _wireSize);
    }
}




float Coil::getRelativeHome(float homePosition, bool dir)
{
  float tmp_value = M_getDisplacement();

  if(!dir){ tmp_value *= -1; }

  return tmp_value += homePosition;
}




bool Coil::winding(bool isNewCoil, float *homingPosition)
{
  // For stopping winding
  bool run = true;
  // For computing
  bool isCoilFastest = 0;
  uint16_t nbPass = 0;
  uint16_t steps = 0;
  // For active setting : speed and display
  bool refresh = false;
  int8_t old_speedPercent = _speedPercent;
  uint16_t speedSet = _speed;
  uint32_t lastMicrosAcc = 0;
  uint32_t oldTimeRefresh = 0;

  // Set start speed with minimal speed
  _speed = _minSpeed;
  // Compute steps by turns for one layer
  isCoilFastest = computeWinding(_coilLength, _wireSize, &nbPass, &steps);
  // Set all target values
  M_setWindingDisplacement(nbPass, steps, _coilTurns, STEPS_PER_TR, isCoilFastest);
  // Set start counter values
  if(isNewCoil)
    {
      M_setState(false, 0, 0, 0, 0);
    }
  else
    {
      M_setState(true, _saveCarrPass, _saveCarrStepPerPass, _saveCoilTr, _saveCoilStepPerTr);
      _windingSense       = _saveCoilDir;
      _carriageStartSense = _saveCarrDir;
    }
  // Set state of motors
  M_setMotors(COIL, _windingSense, CARRIAGE, _carriageStartSense, _minSpeed);
  // Start interrupt routine
  M_start();

  // Display current setting of winding
  _Display->engineWindingValue(_coilLength, _wireSize, _coilTurns, M_getCoilTr());

  while( !M_getWindingStatus() && run)
    {
      if( suspend() == true)
	{
	  run = false;

	  M_stop();

	  M_getState(&_saveCarrPass, &_saveCarrStepPerPass, &_saveCarrDir,
		     &_saveCoilTr, &_saveCoilStepPerTr, &_saveCoilDir);
	}
      else
	{
	  acceleration(speedSet, &_speed, &lastMicrosAcc);

	  if(updateSpeed(&old_speedPercent, &speedSet, 0))
	    {
	      oldTimeRefresh = millis();
	      refresh = true;
	    }

	  refreshDisplay(&refresh, &oldTimeRefresh);

	  _Display->windingGetTurns(_coilTurns, M_getCoilTr());
	}
    }

  if(run == false) return false;
  else return true;
}



/******************************************************************************
 * brief   : Travel only carriage
 * details : Manage the travel to move only the carriage.
 * Distance must be only positive !
 * Function return relative position from home by setting the
 * pointer "homingPosition".
 ******************************************************************************/
void Coil::runOnlyCarriage(bool dir, float distance, float *homingPosition)
{
  bool run = true;

  uint16_t nbPass = 0;
  uint16_t steps = 0;

  int8_t old_speedPercent = _speedPercent;
  uint16_t speedSet = _speed;
  uint32_t lastMicrosAcc = 0;

  // Acceleration need to have current speed set to minimum.
  _speed = _minSpeed;

  // For homing if displacement is negative we need to pass it in positive.
  if(distance<0 ){ distance *=-1; }

  Serial.print(" ");
  Serial.println("********************");
  Serial.print("dir : "), Serial.println(dir);
  Serial.print("distance : "), Serial.println(distance);
  Serial.print("relative position 1 : "), Serial.println(*homingPosition);

  computeTravel(distance, &nbPass, &steps);
  M_setSimpleDisplacement(TRAVELING, nbPass, steps);

  M_setMotors(0, 0, CARRIAGE, dir, _minSpeed);

  M_setState(false, 0, 0, 0, 0);

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
	  updateSpeed(&old_speedPercent, &speedSet, SIZE_MSG_CURRENT_SPEED);

	  _Display->windingGetDisplacement(distance, M_getDisplacement());
	}
    }

  *homingPosition = getRelativeHome(*homingPosition, dir);

  Serial.print("relative position 2 : "), Serial.println(*homingPosition);
}




void Coil::runOnlyCoil(bool dir, uint16_t turns)
{
  bool run = true;

  int8_t old_speedPercent = _speedPercent;
  uint16_t speedSet = _speed;
  uint32_t lastMicrosAcc = 0;

  _speed = _minSpeed;

  M_setSimpleDisplacement(ROTATION, turns, STEPS_PER_TR);

  M_setMotors(COIL, dir, 0, 0, _minSpeed);

  M_setState(false, 0, 0, 0, 0);

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
	  updateSpeed(&old_speedPercent, &speedSet, SIZE_MSG_CURRENT_SPEED);

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


uint16_t Coil::getCurrentTurns()
{
  return M_getCoilTr();
}


void Coil::getState(uint16_t *p_carrPass, uint16_t *p_carrSteps, bool *p_carrDir,
		    uint16_t *p_coilTr, uint16_t *p_coilSteps, bool *p_coilDir)
{
  *p_carrPass  = _saveCarrPass;
  *p_carrSteps = _saveCarrStepPerPass;
  *p_carrDir   = _saveCarrDir;

  *p_coilTr    = _saveCoilTr;
  *p_coilSteps = _saveCoilStepPerTr;
  *p_coilDir   = _saveCoilDir;
}
