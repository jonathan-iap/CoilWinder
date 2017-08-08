#include "Motor.h"

#define NB_TR 50

#define MAX_SPEED 3
#define ACC 1000

struct motor
{
	bool en = DISABLE;
	bool dir = CLOCK;
	uint16_t stepPerTr = 0;
	uint16_t tr = 0;
	uint16_t stepTic = 0;
	uint16_t speed = 0;
};
struct motor coil, carriage;

uint16_t speedSet = MAX_SPEED;
uint16_t tic = 0;

void motorsInit()
{
	Timer1.initialize(10);
	Timer1.attachInterrupt(engineMotors);
	Timer1.stop();

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
	Timer1.start();
}

void motorsStop()
{
	Timer1.stop();
	WRITE(PIN_COIL_EN, DISABLE);
	WRITE(PIN_CARR_EN, ENABLE);
}

void engineMotors()
{
	tic++;
	coil.stepTic++;

	if(coil.tr != NB_TR)
	{
		if(tic == ACC && coil.speed!=speedSet)
		{
			coil.speed--;
			tic=0;
		}

		if(coil.stepTic >= coil.speed)
		{
			coil.stepTic=0;

			oneStep(COIL, 0);

			if(coil.stepPerTr != PIN_COIL_STEPS_PER_TR)
			{
				coil.stepPerTr++;
			}
			else
			{
				coil.stepPerTr = 0;
				coil.tr++;
			}
		}
	}
	else motorsStop();
}


void oneStep(bool M_coil, bool M_carr)
{
	if(M_coil || M_carr)
	{
		if(M_coil)
		{
			WRITE(PIN_COIL_STEP, HIGH);
			WRITE(PIN_COIL_STEP, LOW);
		}
		if(M_carr)
		{
			WRITE(PIN_CARR_STEP, HIGH);
			WRITE(PIN_CARR_STEP, LOW);
		}
	}
}



// GET and SET functions________________________________________________________
uint16_t getMotorCoilTr()
{
	return coil.tr;
}

void setMotors(bool M_coil, bool M_coilDir, bool M_carr, bool M_carrDir)
{
	if(M_coil)
	{
		WRITE(PIN_COIL_EN, ENABLE);

		if(M_coilDir) coil.dir = CLOCK;
		else coil.dir = C_CLOCK;
	}
	if(M_carr)
	{
		WRITE(PIN_CARR_EN, ENABLE);

		if(M_carrDir) carriage.dir = CLOCK;
		else carriage.dir = C_CLOCK;
	}
}


