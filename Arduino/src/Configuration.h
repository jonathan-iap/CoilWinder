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
#define ENC_STEP	2 // How many you increase value, when you make one step

// Motors settings ------------------------------------------------------------
#define CLOCK		HIGH
#define C_CLOCK		LOW
#define ENABLE		LOW
#define DISABLE		HIGH

// Microsteps
//-------------------------
#define FULL_STEPS	1
#define HALF_STEPS	2
#define QUARTER_STEPS	4
#define EIGHTH_STEPS	8
#define SIXTEENTH_STEPS 16
//-------------------------

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

//#define DELAY_DRIVER  	20 // Activated if is needed by your stepper driver.


// Machine settings ------------------------------------------------------------

// Pitch in mm
//--------------
#define  M3 0.5
#define  M4 0.7
#define  M5 0.8
#define  M6 1
//--------------

#define LEAD_SCREW_PITCH M5 // replace by your own setup.

// Reset value -----------------------------------------------------------------
#define INIT_WIRE	"0.00"
#define INIT_COIL	"000.00"
#define INIT_TURNS	"00000"
#define INIT_MAXSPEED	"0150"
#define INIT_MINSPEED	"1800"
#define INIT_ACC_DELAY	"0500"

// Serial Debug option ---------------------------------------------------------
#define DEBUG // Uncomment to active serial debug.
#define BAUDRATE 9600

#endif /* CONFIGURATIONS_H */
