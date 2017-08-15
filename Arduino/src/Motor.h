#ifndef MOTOR_H
#define MOTOR_H

#include "Arduino.h"
#include "Configuration.h"
#include "Fastio.h"
#include "TimerOne.h"

#define COIL      1
#define CARRIAGE  2

#define ACC 1000

#define MOTOR_INT 10 // motor interrupt in microseconds
#define RPM_TO_INT(rpm) ((600000 / rpm) / STEPS_PER_TR * MOTOR_INT)

#define WINDING		1
#define TRAVELING	2
#define ROTATION	3

void M_init();
void M_start();
void M_stop();
void M_engine();

void oneStep(bool M_coil, bool M_carr);

uint16_t M_getCoilTr();
bool M_getWindingStatus();
float M_getDisplacement();

void M_setMotors(bool M_coil, bool M_coilDir, bool M_carr, bool M_carrDir);
void M_setSpeed(uint16_t speed, uint16_t maxSpeed, uint16_t minSpeed, uint16_t acc);
void M_setDisplacement(uint8_t action, uint16_t pass, uint16_t steps);

void M_updateSpeed(uint16_t speed);

#endif
