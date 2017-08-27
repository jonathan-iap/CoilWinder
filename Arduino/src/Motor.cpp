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

volatile uint8_t engineAction = 0;

volatile uint16_t speedSet = 0;
volatile uint16_t minSpeed = 0;

volatile uint16_t targetPass = 0;
volatile uint16_t stepsPerPass = 0;
volatile uint16_t targetTr = 0;
volatile uint16_t stepsPerTr = 0;

volatile bool isCoilFastest = false;
volatile bool isFinished = false;

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


  isFinished = false;

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

  switch (engineAction)
  {
    case WINDING : //__________________________________________________________
      {
	switch(isCoilFastest)
	{
	  case true :
	    {
	      if(coil.tr != targetTr)
		{
		  if(coil.tic >= speedSet)
		    {
		      coil.tic=0;
		      oneStep(ENABLE , DISABLE);

		      if(coil.steps != stepsPerTr)
			{
			  coil.steps++;

			  if(carriage.steps != stepsPerPass)
			    {
			      carriage.steps++;
			      oneStep(DISABLE, ENABLE);
			    }
			}
		      else
			{
			  carriage.steps = 0;
			  carriage.tr++;
			  coil.steps = 0;
			  coil.tr++;

			  if(carriage.tr == targetPass)
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
		  isFinished = true;
		}

	      break;
	    }
	  case false :
	    {
	      if(coil.tr != targetTr)
		{
		  if(carriage.tic >= speedSet)
		    {
		      carriage.tic=0;
		      oneStep(DISABLE , ENABLE);

		      if(carriage.steps != stepsPerPass)
			{
			  carriage.steps++;

			  if(coil.steps != stepsPerTr)
			    {
			      coil.steps++;
			      oneStep(ENABLE, DISABLE);
			    }
			}
		      else
			{
			  carriage.steps = 0;
			  carriage.tr++;
			  coil.steps = 0;
			  coil.tr++;

			  if(carriage.tr == targetPass)
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
		  isFinished = true;
		}
	      break;
	    }
	}
	break;
      }
    case TRAVELING : //________________________________________________________
      {
	if(carriage.tr != targetPass && stepsPerPass > 0)
	  {
	    if(carriage.tic >= speedSet)
	      {
		carriage.tic=0;

		oneStep(coil.en, carriage.en);

		if(carriage.steps != stepsPerPass)
		  {
		    carriage.steps++;
		  }
		else
		  {
		    carriage.steps = 0;
		    carriage.tr++;
		  }
	      }
	    else{}
	  }
	else
	  {
	    M_stop();
	    isFinished = true;
	  }
	break;
      }
    case ROTATION : //_________________________________________________________
      {
	if(coil.tr != targetPass)
	  {
	    if(coil.tic >= speedSet)
	      {
		coil.tic=0;

		oneStep(coil.en , carriage.en);

		if(coil.steps != stepsPerPass)
		  {
		    coil.steps++;
		  }
		else
		  {
		    coil.steps = 0;
		    coil.tr++;
		  }
	      }
	    else{}
	  }
	else
	  {
	    M_stop();
	    isFinished = true;
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



// GET and SET functions________________________________________________________
uint16_t M_getCoilTr()
{
  return  coil.tr;
}

bool M_getWindingStatus()
{
  return isFinished;
}

float M_getDisplacement()
{
  float displacement = (((((float)stepsPerPass * (float)carriage.tr) + (float)carriage.steps)
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

  minSpeed = M_setSpeed(startSpeed);
}

uint16_t M_setSpeed(uint16_t speed)
{
  speedSet = RPM_TO_INT((float)speed);
  return speedSet;
}

void M_invertSense()
{
  // speedSet = minSpeed; // reduce speed to change sense
  WRITE(PIN_CARR_DIR, !READ(PIN_CARR_DIR));
}

void M_setSimpleDisplacement(uint8_t action, uint16_t pass, uint16_t steps)
{
  engineAction  = action;
  targetPass    = pass;
  stepsPerPass  = steps;
}

void M_setWindingDisplacement(uint16_t pass, uint16_t steps, uint16_t tr,
			      uint16_t stepsTr, bool fastest)
{
  engineAction  = WINDING;
  targetPass    = pass;
  stepsPerPass  = steps;
  targetTr      = tr;
  stepsPerTr    = stepsTr;
  isCoilFastest = fastest;

  Serial.println(" ");
  Serial.println("****** SetWinding ******");
  Serial.print("targetPass : "), Serial.println(targetPass);
  Serial.print("stepsPerPass : "), Serial.println(stepsPerPass);
  Serial.print("targetTr : "), Serial.println(targetTr);
  Serial.print("stepsPerTr : "), Serial.println(stepsPerTr);
  Serial.print("isCoilFastest : "), Serial.println(isCoilFastest);
  Serial.println("*****************");
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
