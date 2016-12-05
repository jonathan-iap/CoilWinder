/*
 * Display.h
 *
 *  Created on: 5 nov. 2016
 *      Author: jo
 */

#ifndef SRC_DISPLAY_H_
#define SRC_DISPLAY_H_

#include "Arduino.h"
#include "Configuration.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"

#define IconLeft 	1
#define IconRight	2
#define IconBack	3
#define IconEnter	4
#define IconValid	5

#define MSG_WIRE_SIZE	"Wire size in mm"
#define MSG_COIL_LENGTH	"Coil length in mm"
#define MSG_TURNS	"Turns in tr"
#define MSG_MAX_SPEED	"MAX speed"
#define MSG_MIN_SPEED	"MIN speed"
#define MSG_ACC_DELAY	"Acc duration"

#define MSG_VALID 	"Valid:"
#define SIZE_MSG_VALID 	sizeof(MSG_VALID)
#define MSG_SAVE 	"Save?Y/N"
#define SIZE_MSG_SAVE 	sizeof(MSG_SAVE)

class Display
{
  // Public variables -----------------------------------------------------------
public :

  uint8_t left[8] = {
      0b00010,
      0b00110,
      0b01110,
      0b11110,
      0b01110,
      0b00110,
      0b00010,
      0b00000
  };

  uint8_t right[8] = {
      0b01000,
      0b01100,
      0b01110,
      0b01111,
      0b01110,
      0b01100,
      0b01000,
      0b00000
  };

  uint8_t back[8] = {
      0b00000,
      0b00100,
      0b01100,
      0b11111,
      0b01101,
      0b00101,
      0b00001,
      0b00000
  };

  uint8_t enter[8] = {
      0b00000,
      0b10000,
      0b10100,
      0b10110,
      0b11111,
      0b00110,
      0b00100,
      0b00000
  };

  uint8_t valid[8] = {
      0b00000,
      0b00000,
      0b00001,
      0b00010,
      0b10100,
      0b01000,
      0b00000,
      0b00000
  };

  // Public functions -----------------------------------------------------------
public:
  Display(LiquidCrystal_I2C *p_lcd);
  const void begin();
  const void version();

private:
  LiquidCrystal_I2C *_lcd;
};

#endif /* SRC_DISPLAY_H_ */
