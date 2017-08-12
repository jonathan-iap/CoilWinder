#include "Motor.h"

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

uint16_t speedSet = 0;
uint16_t tic = 0;
uint16_t targetTurns = 0;
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
	Timer1.attachInterrupt(engineMotors);
}

void motorsStop()
{
	Timer1.detachInterrupt();

	digitalWrite(PIN_COIL_EN, DISABLE);
	digitalWrite(PIN_CARR_EN, DISABLE);
}

void engineMotors()
{
	tic++;
	coil.stepTic++;

	if(coil.tr != targetTurns)
	{
//		if(tic == ACC && coil.speed!=speedSet)
//		{
//			coil.speed--;
//			tic=0;
//		}

		if(coil.stepTic >= coil.speed)
		{
			coil.stepTic=0;

			oneStep(COIL, CARRIAGE);

			if(coil.stepPerTr != STEPS_PER_TR)
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
	else
	{
		motorsStop();
		isFinished = true;
	}

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

bool getWindingStatus()
{
	return isFinished;
}

void setMotors(bool M_coil, bool M_coilDir, bool M_carr, bool M_carrDir, uint16_t maxWindingSpeed)
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

	coil.speed = RPM_TO_INT(maxWindingSpeed);
	Serial.print("RPM : "), Serial.print(coil.speed);
}

void setDisplacement(uint16_t tr)
{
	targetTurns = tr;
}

