/*
 * Configuration.h
 *
 *  Created on: 26 oct. 2016
 *      Author: jo
 */

#ifndef CONFIGURATIONS_H
#define CONFIGURATIONS_H

// LCD screen definition ------------------------------------------------------
#define LCD_CHARS 16
#define LCD_LINES  2

// Rotary encoder properties --------------------------------------------------
#define ENC_PIN_A	3 // The order declaration(pin A, B) determine the direction.
#define ENC_PIN_B	2 //
#define ENC_PIN_SW	4 // Click switch
#define ENC_STEP	2 // How many you increase value when you make one step

// Motors settings ------------------------------------------------------------
#define CLOCK		HIGH
#define C_CLOCK		LOW
#define ENABLE		LOW
#define DISABLE		HIGH
enum
{
  FULL_STEPS		= 1,
  HALF_STEPS		= 2,
  QUARTER_STEPS		= 4,
  EIGHTH_STEPS		= 8,
  SIXTEENTH_STEPS	= 16
};

// Motor for winding
#define M1_DIR	10
#define M1_STEP	9
#define M1_EN	8
#define M1_initialStepPerTr 200
#define M1_STEPS_PER_TR (M1_initialStepPerTr * EIGHTH_STEPS)

// Mode for carriage
#define M2_DIR	7
#define M2_STEP	6
#define M2_EN	5
#define M2_initialStepPerTr 200
#define M2_STEPS_PER_TR (M2_initialStepPerTr * EIGHTH_STEPS)

#define ACC	1	// Acceleration step increment.
//#define DELAY_DRIVER  	20 // Active if is need by your stepper driver.


// Machine settings ------------------------------------------------------------
// M5 = 0.8 mm
#define LEAD_SCREW_PITCH 0.8

// Serial Debug option ---------------------------------------------------------
//#define DEBUG // Uncomment to active serial debug.
#define BAUDRATE 9600

#endif /* CONFIGURATIONS_H */
