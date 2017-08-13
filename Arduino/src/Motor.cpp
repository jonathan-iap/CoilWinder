#include "Motor.h"

struct motor
{
  bool en = DISABLE;
  bool dir = CLOCK;
  uint16_t stepPerTr = 0;
  uint16_t tr = 0;
  uint16_t stepTic = 0;
};
struct motor coil, carriage;

bool engineAction = WINDING;
uint16_t speedSet = 0;
uint16_t tic = 0;
uint16_t targetPass = 0;
uint16_t stepsPerPass = 0;
bool isFinished = false;

void motorsInit()
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

void motorsStart()
{
  coil.stepPerTr = 0;
  coil.stepTic = 0;
  coil.tr = 0;

  carriage.stepPerTr = 0;
  carriage.stepTic = 0;
  carriage.tr = 0;

  Timer1.attachInterrupt(engineMotors);
}

void motorsStop()
{
  Timer1.detachInterrupt();

  WRITE(PIN_COIL_EN, DISABLE);
  WRITE(PIN_CARR_EN, DISABLE);
}

void engineMotors()
{
  tic++;
  coil.stepTic++;
  carriage.stepTic++;

  if(engineAction == WINDING)
    {

    }
  else
    {
      if(carriage.tr != targetPass && stepsPerPass > 0)
	{
	  if(carriage.stepTic >= speedSet)
	    {
	      carriage.stepTic=0;

	      oneStep((coil.en = DISABLE), carriage.en);

	      if(carriage.stepPerTr != stepsPerPass)
		{
		  carriage.stepPerTr++;
		}
	      else
		{
		  carriage.stepPerTr = 0;
		  carriage.tr++;
		}
	    }
	  else{}
	}
      else
	{
	  motorsStop();
	  isFinished = true;
	}
    }

  //	if(coil.tr != targetTurns)
  //	{
  //		//		if(tic == ACC && coil.speed!=speedSet)
  //		//		{
  //		//			coil.speed--;
  //		//			tic=0;
  //		//		}
  //
  //		if(coil.stepTic >= speedSet)
  //		{
  //			coil.stepTic=0;
  //
  //			oneStep(coil.en, (carriage.en = DISABLE));
  //
  //			if(coil.stepPerTr != STEPS_PER_TR)
  //			{
  //				coil.stepPerTr++;
  //			}
  //			else
  //			{
  //				coil.stepPerTr = 0;
  //				coil.tr++;
  //			}
  //		}
  //		else{}
  //	}
  //	else
  //	{
  //		motorsStop();
  //		isFinished = true;
  //	}

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
uint16_t getMotorCoilTr()
{
  return coil.tr;
}

bool getWindingStatus()
{
  return isFinished;
}

void setMotors(bool M_coil, bool M_coilDir, bool M_carr, bool M_carrDir, uint16_t maxWindingSpeed)
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

  speedSet = RPM_TO_INT(maxWindingSpeed);
}

void setDisplacement(bool action, uint16_t pass, uint16_t steps)
{
  engineAction = action;
  targetPass = pass;
  stepsPerPass = steps;
}

