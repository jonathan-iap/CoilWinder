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
#define ENC_PIN_BTN	4 // Click switch
#define ENC_STEP	2 // How many you increase value, when you make one step

// Motors settings ------------------------------------------------------------
#define CLOCK		HIGH
#define C_CLOCK		LOW
#define ENABLE		LOW
#define DISABLE		HIGH

// Steps
//-------------------------
#define MOTOR_STEPS 200
// If you change micros steps you need to change MaxSpeed !
#define FULL_STEPS	1
#define HALF_STEPS	2
#define QUARTER_STEPS	4
#define EIGHTH_STEPS	8
#define SIXTEENTH_STEPS 16
// Setting for micros stepping (make FULL_STEP if you don't use micros stepping)
#define STEPS_PER_TR (MOTOR_STEPS * SIXTEENTH_STEPS)
//-------------------------

// Motor for winding
#define PIN_COIL_DIR	10
#define PIN_COIL_STEP	9
#define PIN_COIL_EN	8

// Mode for carriage
#define PIN_CARR_DIR	7
#define PIN_CARR_STEP	6
#define PIN_CARR_EN	5


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
#define INIT_WIRE	"0.00" 		// in mm
#define INIT_COIL	"000.00"	// in mm
#define INIT_TURNS	"00000"		// in Tr
#define INIT_MAXSPEED	"0200"  // in RPM (tr/min)
#define INIT_MINSPEED	"0001"  // in RPM (tr/min)
#define INIT_ACC_DELAY	"0400"
#define INIT_ACC_INCR	"0020"
#define INIT_W_SENSE	C_CLOCK
#define INIT_C_SENSE	CLOCK

// Displacement value ---------------------------------------------------------------
#define INIT_MOV_CAR	"00.00"
#define INIT_MOV_COIL	"0000"

// Serial Debug option ---------------------------------------------------------
#define DEBUG // Uncomment to active serial debug.
#define BAUDRATE 9600

#endif /* CONFIGURATIONS_H */
