/*
 * Configuration.h
 *
 *  Created on: 26 oct. 2016
 *      Author: jo
 */

#ifndef CONFIGURATIONS_H
#define CONFIGURATIONS_H

#define CLOCK		HIGH
#define C_CLOCK		LOW
#define ENABLE		LOW
#define DISABLE		HIGH
//#define DELAY_DRIVER  	20 // Active if is need by your stepper driver.

/* Motor for winding */
#define M1_DIR	10
#define M1_STEP	9
#define M1_EN	8
#define M1_STEPS_PER_TR 200

/* Mode for carriage */
#define M2_DIR	7
#define M2_STEP	6
#define M2_EN	5
#define M2_STEPS_PER_TR 200

/* Acceleration step*/
#define ACC	1

#define LEAD_SCREW_PITCH 0.8

#endif /* CONFIGURATIONS_H */
