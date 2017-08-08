#ifndef MOTOR_H
#define MOTOR_H

#include "Arduino.h"
#include "Configuration.h"
#include "Fastio.h"
#include "TimerOne.h"

#define COIL      1
#define CARRIAGE  2

void motorsInit();
void motorsStart();
void motorsStop();
void engineMotors();

uint16_t getMotorCoilTr();

void setMotors(bool M_coil, bool M_coilDir, bool M_carr, bool M_carrDir);
void oneStep(bool M_coil, bool M_carr);

#endif
