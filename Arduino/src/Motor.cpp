#include "Motor.h"

struct motor
{
  bool en = DISABLE;
  bool dir = CLOCK;
  uint16_t steps = 0;
  uint16_t tr = 0;
  uint16_t tic = 0;
};
struct motor coil, carriage;

uint8_t engineAction = 0;

uint16_t speedSet = 0;
uint16_t tic = 0;
uint16_t targetPass = 0;
uint16_t stepsPerPass = 0;

uint16_t Int_maxSpeed = 0;
uint16_t Int_minSpeed = 0;
uint16_t Int_acc = 0;
uint16_t accSpeed = 0;

bool isFinished = false;

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
  coil.steps = 0;
  coil.tic = 0;
  coil.tr = 0;

  carriage.steps = 0;
  carriage.tic = 0;
  carriage.tr = 0;

  isFinished = false;

  Timer1.attachInterrupt(M_engine);
}

void M_stop()
{
  Timer1.detachInterrupt();

  WRITE(PIN_COIL_EN, DISABLE);
  WRITE(PIN_CARR_EN, DISABLE);
}

void M_engine()
{
  tic++;
  coil.tic++;
  carriage.tic++;

  switch (engineAction)
  {
    case WINDING :
      {
	break;
      }
    case TRAVELING :
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
    case ROTATION :
      {
	if(coil.tr != targetPass)
	  {
//	    if(tic == Int_acc && coil.tic!=speedSet)
//	      {
//		//coil.speed--;
//		tic=0;
//	      }
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


void M_setMotors(bool M_coil, bool M_coilDir, bool M_carr, bool M_carrDir)
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
}

void M_setSpeed(uint16_t speed, uint16_t maxSpeed, uint16_t minSpeed, uint16_t acc)
{
//speedSet = (float)RPM_TO_INT((float)speed);
//Serial.print("speedSet : "), Serial.print(speedSet);

M_updateSpeed(speed);
}


void M_setDisplacement(uint8_t action, uint16_t pass, uint16_t steps)
{
  engineAction = action;
  targetPass = pass;
  stepsPerPass = steps;
}


void M_updateSpeed(uint16_t speed)
{
  float tmp_speedSet = RPM_TO_INT((float)speed);
  speedSet = (uint16_t)tmp_speedSet;
}
