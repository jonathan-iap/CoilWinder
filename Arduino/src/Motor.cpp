#include "Motor.h"
#include "Configuration.h"
#include "Fastio.h"

Motor::Motor()
{}

Motor::~Motor()
{}

void Motor::begin()
{
  SET_OUTPUT(M1_DIR);
  SET_OUTPUT(M1_STEP);
  SET_OUTPUT(M1_EN);

  SET_OUTPUT(M2_DIR);
  SET_OUTPUT(M2_STEP);
  SET_OUTPUT(M2_EN);

  coil_disable();
  carriage_disable();
}

/******************************************************************************
 * brief   : Enable stepper
 ******************************************************************************/
void Motor::coil_enable()
{
  WRITE(M1_EN, ENABLE);
}

void Motor::carriage_enable()
{
  WRITE(M2_EN, ENABLE);
}

/******************************************************************************
 * brief   : Disable stepper
 ******************************************************************************/
void Motor::coil_disable()
{
  WRITE(M1_EN, DISABLE);
}

void Motor::carriage_disable()
{
  WRITE(M2_EN, DISABLE);
}

/******************************************************************************
 * brief   : Motor advance.
 * details : Verified that enable pins are ok and make one step in the direction
 * you want.
 ******************************************************************************/
void Motor::coil_oneStep(bool direction)
{
  if(READ(M1_EN) != ENABLE) coil_enable();

  if(READ(M1_DIR) != direction) WRITE(M1_DIR, direction);

  WRITE(M1_STEP, HIGH);
#ifdef  DELAY_DRIVER
  delayMicroseconds(DELAY_DRIVER);
#endif
  WRITE(M1_STEP, LOW);
}

void Motor::carriage_oneStep(bool direction)
{
  if(READ(M2_EN) != ENABLE) carriage_enable();

  if(READ(M2_DIR) != direction) WRITE(M2_DIR, direction);

  WRITE(M2_STEP, HIGH);
#ifdef  DELAY_DRIVER
  delayMicroseconds(DELAY_DRIVER);
#endif
  WRITE(M2_STEP, LOW);
}

