#include "Motor.h"

struct motor
{
  volatile bool en = DISABLE;
  volatile bool dir = CLOCK;
  volatile uint16_t steps = 0;
  volatile uint16_t tr = 0;
  volatile uint16_t tic = 0;
};
struct motor coil, carriage;

volatile uint8_t M_engineAction = 0;

volatile uint16_t M_speedSet = 0;
volatile uint16_t M_minSpeed = 0;

volatile uint16_t M_targetPass = 0;
volatile uint16_t M_stepsPerPass = 0;
volatile uint16_t M_targetTr = 0;
volatile uint16_t M_stepsPerTr = 0;

volatile uint16_t M_stepsInterval = 0;
volatile uint16_t M_stepsRest = 0;
volatile uint16_t M_stepsAdd = 0;

volatile uint16_t M_countInterval = 0;

volatile bool M_isCoilFastest = false;
volatile bool M_isFinished = false;
volatile bool M_isSuperior = false;
volatile bool M_isStepAddMode = false;

void M_init()
{
  Timer1.initialize(MOTOR_INT);

  pinMode(PIN_COIL_DIR , OUTPUT);
  pinMode(PIN_COIL_STEP, OUTPUT);
  pinMode(PIN_COIL_EN  , OUTPUT);

  pinMode(PIN_CARR_DIR , OUTPUT);
  pinMode(PIN_CARR_STEP, OUTPUT);
  pinMode(PIN_CARR_EN  , OUTPUT);

  digitalWrite(PIN_COIL_EN, DISABLE);
  digitalWrite(PIN_CARR_EN, DISABLE);
}

void M_start()
{
  M_isFinished = false;

  Timer1.attachInterrupt(M_engine);
}


void M_stop()
{
  Timer1.detachInterrupt();

  WRITE(PIN_COIL_EN, DISABLE);
  WRITE(PIN_CARR_EN, DISABLE);
}


/******************************************************************************
 * brief   : Engine for motor
 * details : Divide the timer tic to create accurate displacement.
 *
 * Fix timer callback each 10us :           Tic++...
 *   _    _    _    _    _    _    _    _   v_    _    _    _    _    _    _    _
 * _| |__| |__| |__| |__| |__| |__| |__| |__| |__| |__| |__| |__| |__| |__| |__| |
 *
 * Step for one motor constant speed, exemple : one step at each 4 tics
 *   _              _              _              _              _              _
 * _| |____________| |____________| |____________| |____________| |____________| |
 *
 * For acceleration step per tic is decrease depending a constant delay:
 *   _                   _              _         _         _         _         _
 * _| |______5Tic_______| |____4Tic____| |__3Tic_| |__3Tic_| |_3Tic__| |_3Tic__| |
 *
 *<------delay---------><------delay---------><------delay---------><------delay--------->
 *
 * Attention timer have a response curve like that :
 *
 * | timer frequency
 * |
 * |
 * |
 * |
 * | *
 * |
 * |
 * |
 * |
 * |
 * | *
 * |
 * |
 * |
 * |
 * |   *
 * |
 * |      *
 * |
 * |           *
 * |
 * |                   *
 * |                                     *
 * |                                                                *
 * |------------------------------------------------------------------------------------>
 * Low value timer						high value timer
 *  1                                                               65535
 *
 * This give an acceleration like this if the value is to low
 *
 * speed
 *   |           |________
 *   |          ||
 *   |       ___||
 *   |    __|    |
 *   |   |       |
 *   |  /        |
 * __|/__________|_________
 *               |     time
 *            end acc
 *
 * A smooth acceleration look like this :
 *
 * speed
 *   |           |__________
 *   |          /|
 *   |        /  |
 *   |      /    |
 *   |    /      |
 *   |  /        |
 *___|/__________|__________
 *               |      time
 *            end acc
 *
 *
 ******************************************************************************/
void M_engine()
{
  coil.tic++;
  carriage.tic++;

  switch (M_engineAction)
  {
    case WINDING : //________________________________________________________________________________________________________________________________
      {
	switch(M_isCoilFastest)
	{
	  case true : // Coil have a numbers of steps superior to carriage.
	    {
	      if(coil.tr != M_targetTr)
		{
		  if(coil.tic >= M_speedSet)
		    {
		      coil.tic=0;

		      if(coil.steps != M_stepsPerTr)
			{
			  oneStep(ENABLE , DISABLE); // Coil step depending tic interval.
			  ++coil.steps;
			  ++M_countInterval;

			  if(carriage.steps != M_stepsPerPass)
			    {
			      switch(M_isSuperior)
			      {
				case true : // Blank steps are superiors to half steps per pass.
				  {
				    // Second step : normal distribution.
				    if(carriage.steps >= M_stepsRest && !M_isStepAddMode)
				      {
					M_isStepAddMode = true;
					--M_stepsInterval;
				      }
				    // First step : add a blank step to interval.
				    if(M_countInterval > M_stepsInterval)
				      {
					oneStep(DISABLE, ENABLE);
					++carriage.steps;
					M_countInterval = 0;
				      }
				    break;
				  }
				case false : // Blank steps are inferiors to half steps per pass.
				  {
				    // Second step : add a step to interval.
				    if(carriage.steps > M_stepsAdd && !M_isStepAddMode)
				      {
					M_isStepAddMode = true;
					++M_stepsInterval;
				      }
				    // First step : normal distribution.
				    if(M_countInterval <= M_stepsInterval)
				      {
					oneStep(DISABLE, ENABLE);
					++carriage.steps;
				      }
				    else
				      {
					M_countInterval = 0;
				      }
				  }
			      }
			    }
			}
		      else // Reset counter for next turn.
			{
			  updateCounter();

			  if(carriage.tr == M_targetPass)
			    {
			      carriage.tr = 0;
			      M_invertSense();
			    }
			}
		    }
		}
	      else // End of winding
		{
		  M_stop();
		  M_isFinished = true;
		}

	      break;
	    }
	  case false : // Coil have a numbers of steps inferior to carriage.
	    {
	      // Algorithm is the same that before but motors are inverted.
	      if(coil.tr != M_targetTr)
		{
		  if(carriage.tic >= M_speedSet)
		    {
		      carriage.tic=0;

		      if(carriage.steps != M_stepsPerPass)
			{
			  oneStep(DISABLE , ENABLE);
			  ++carriage.steps;
			  ++M_countInterval;

			  if(coil.steps != M_stepsPerTr)
			    {
			      if(M_isSuperior == true)
				{
				  if(coil.steps >= M_stepsRest && !M_isStepAddMode)
				    {
				      M_isStepAddMode = true;
				      --M_stepsInterval;
				    }

				  if(M_countInterval > M_stepsInterval)
				    {
				      oneStep(ENABLE, DISABLE);
				      ++coil.steps;
				      M_countInterval = 0;
				    }
				}
			      else
				{
				  if(coil.steps > M_stepsAdd && !M_isStepAddMode)
				    {
				      M_isStepAddMode = true;
				      ++M_stepsInterval;
				    }

				  if(M_countInterval <= M_stepsInterval)
				    {
				      oneStep(ENABLE, DISABLE);
				      ++coil.steps;
				    }
				  else
				    {
				      M_countInterval = 0;
				    }
				}
			    }
			}
		      else
			{
			  updateCounter();

			  if(carriage.tr == M_targetPass)
			    {
			      carriage.tr = 0;
			      M_invertSense();
			    }
			}
		    }
		}
	      else
		{
		  M_stop();
		  M_isFinished = true;
		}
	      break;
	    }
	}
	break;
      }
    case TRAVELING : //______________________________________________________________________________________________________________________________
      {
	if(carriage.tr != M_targetPass && M_stepsPerPass > 0)
	  {
	    if(carriage.tic >= M_speedSet)
	      {
		carriage.tic=0;

		oneStep(coil.en, carriage.en);

		if(carriage.steps != M_stepsPerPass)
		  {
		    ++carriage.steps;
		  }
		else
		  {
		    carriage.steps = 0;
		    ++carriage.tr;
		  }
	      }
	    else{}
	  }
	else
	  {
	    M_stop();
	    M_isFinished = true;
	  }
	break;
      }
    case ROTATION : //_______________________________________________________________________________________________________________________________
      {
	if(coil.tr != M_targetPass)
	  {
	    if(coil.tic >= M_speedSet)
	      {
		coil.tic=0;

		oneStep(coil.en , carriage.en);

		if(coil.steps != M_stepsPerPass)
		  {
		    ++coil.steps;
		  }
		else
		  {
		    coil.steps = 0;
		    ++coil.tr;
		  }
	      }
	    else{}
	  }
	else
	  {
	    M_stop();
	    M_isFinished = true;
	  }
	break;
      }
  }
}


void oneStep(bool M_coil_en, bool M_carr_en)
{
  if(M_coil_en == ENABLE)
    {
      WRITE(PIN_COIL_STEP, HIGH);
      WRITE(PIN_COIL_STEP, LOW);
    }
  if(M_carr_en == ENABLE)
    {
      WRITE(PIN_CARR_STEP, HIGH);
      WRITE(PIN_CARR_STEP, LOW);
    }
}

void updateCounter()
{
  M_isStepAddMode = false;

  carriage.steps = 0;
  ++carriage.tr;
  coil.steps = 0;
  ++coil.tr;


  switch(M_isSuperior)
  {
    case true:
      {
	++M_stepsInterval;
	M_countInterval = M_stepsInterval+1;
	break;
      }
    case false:
      {
	--M_stepsInterval;
	M_countInterval = 0;
	break;
      }
  }
}

// GET and SET functions________________________________________________________
uint16_t M_getCoilTr()
{
  return  coil.tr;
}




bool M_getWindingStatus()
{
  return M_isFinished;
}




float M_getDisplacement()
{
  float displacement = (((((float)M_stepsPerPass * (float)carriage.tr) + (float)carriage.steps)
      / (float)STEPS_PER_TR) * (float)LEAD_SCREW_PITCH);

  return displacement;
}




float M_getDisplacement(uint16_t carrstartSense)
{
  float displacement = (((((float)M_stepsPerPass * (float)carriage.tr) + (float)carriage.steps)
      / (float)STEPS_PER_TR) * (float)LEAD_SCREW_PITCH);

  return displacement;
}




void M_setMotors(bool M_coil, bool M_coilDir, bool M_carr, bool M_carrDir, uint16_t startSpeed)
{
  if(M_coil)
    {
      coil.en = ENABLE;

      if(M_coilDir == CLOCK) coil.dir = CLOCK;
      else coil.dir = C_CLOCK;
    }
  else coil.en = DISABLE;

  if(M_carr)
    {
      carriage.en = ENABLE;

      if(M_carrDir == CLOCK) carriage.dir = CLOCK;
      else carriage.dir = C_CLOCK;
    }
  else carriage.en = DISABLE;

  WRITE(PIN_COIL_EN, coil.en);
  WRITE(PIN_COIL_DIR, coil.dir);

  WRITE(PIN_CARR_EN, carriage.en);
  WRITE(PIN_CARR_DIR, carriage.dir);

  M_minSpeed = M_setSpeed(startSpeed);
}

uint16_t M_setSpeed(uint16_t speed)
{
  M_speedSet = RPM_TO_INT((float)speed);
  return M_speedSet;
}

void M_invertSense()
{
  // speedSet = minSpeed; // reduce speed to change sense
  WRITE(PIN_CARR_DIR, !READ(PIN_CARR_DIR));
}


void M_setSimpleDisplacement(uint8_t action, uint16_t pass, uint16_t steps)
{
  M_engineAction  = action;
  M_targetPass    = pass;
  M_stepsPerPass  = steps;
}


void M_setWindingDisplacement(uint16_t pass, uint16_t steps, uint16_t tr,
			      uint16_t stepsTr, bool fastest)
{
  M_engineAction  = WINDING;
  M_targetPass    = pass;
  M_stepsPerPass  = steps;
  M_targetTr      = tr;
  M_stepsPerTr    = stepsTr;
  M_isCoilFastest = fastest;
}


void M_setWindingCounter(bool isNewCoil, bool isSuperior, uint16_t interval, uint16_t rest, uint16_t add)
{
  M_stepsRest     = rest;
  M_stepsAdd      = add ;
  M_isSuperior    = isSuperior; // nb of step to distribute > to nb of step to make
  M_isStepAddMode = false;

  // counter
  isSuperior ? M_stepsInterval = interval+1 : M_stepsInterval = interval;

  if(isNewCoil)
    {
      isSuperior ? M_countInterval = M_stepsInterval+1 : M_countInterval = 0; // start with one step or not
    }

  //  Serial.print("isNewCoil : "), Serial.println(isNewCoil);
  //  Serial.print("M_countInterval : "), Serial.println(M_countInterval);
  //  Serial.print("isSuperior : "), Serial.println(M_isSuperior);
  //  Serial.print("stepRest : "), Serial.println(M_stepsRest);
  //  Serial.print("stepInterval : "), Serial.println(M_stepsInterval);
  //  Serial.print("nbAddStep : "), Serial.println(M_stepsAdd);
}


void M_setState(bool isResume, uint16_t carrPass, uint16_t carrSteps, uint16_t coilTr, uint16_t coilSteps)
{
  if(isResume)
    {
      carriage.tr = carrPass;
      carriage.steps = carrSteps;
      coil.tr = coilTr;
      coil.steps = coilSteps;
    }
  else
    {
      coil.steps = 0;
      coil.tr = 0;
      carriage.steps = 0;
      carriage.tr = 0;
    }

  carriage.tic = 0;
  coil.tic = 0;
}


void M_getState(uint16_t *p_carrPass, uint16_t *p_carrSteps, bool *p_carrDir,
		uint16_t *p_coilTr, uint16_t *p_coilSteps, bool *p_coilDir)
{
  *p_carrPass  = carriage.tr;
  *p_carrSteps = carriage.steps;
  *p_carrDir   = READ(PIN_CARR_DIR);

  *p_coilTr    = coil.tr;
  *p_coilSteps = coil.steps;
  *p_coilDir   = READ(PIN_COIL_DIR);
}
