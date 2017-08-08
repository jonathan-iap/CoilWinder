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

	pinMode(M1_DIR , OUTPUT);
	pinMode(M1_STEP, OUTPUT);
	pinMode(M1_EN  , OUTPUT);

	pinMode(M2_DIR , OUTPUT);
	pinMode(M2_STEP, OUTPUT);
	pinMode(M2_EN  , OUTPUT);

	digitalWrite(M1_EN, DISABLE);
	digitalWrite(M2_EN, DISABLE);
}

void motorsStart()
{
	tic=0;

	coil.stepTic = 0;
	coil.stepPerTr = 0;
	coil.tr = 0;
	coil.speed = 50;

	WRITE(M1_EN, ENABLE);
	Timer1.start();
}

void motorsStop()
{
	Timer1.stop();
	WRITE(M1_EN, DISABLE);
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

			if(coil.stepPerTr != M1_STEPS_PER_TR)
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
	if(M_coil && M_carr)
	{
		WRITE(M1_STEP, HIGH);
		WRITE(M1_STEP, LOW);
		WRITE(M2_STEP, HIGH);
		WRITE(M2_STEP, LOW);
	}
	else if(M_coil)
	{
		WRITE(M1_STEP, HIGH);
		WRITE(M1_STEP, LOW);
	}
	else
	{
		WRITE(M2_STEP, HIGH);
		WRITE(M2_STEP, LOW);
	}
}



// GET and SET functions________________________________________________________
uint16_t getMotorCoilTr()
{
	return coil.tr;
}

void setMotors(bool M_coil, bool M_coilDir, bool M_carr, bool M_carrDir)
{

}
