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
#include "Function.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"

#define IconLeft 	1
#define IconRight	2
#define IconBack	3
#define IconEnter	4
#define IconValid	5
#define IconBlock	6

#define MSG_WIRE_SIZE	"Wire size in mm"
#define MSG_COIL_LENGTH	"Length in mm"
#define MSG_TURNS	"Turns in tr"
#define MSG_MAX_SPEED	"MAX speed in us"
#define MSG_MIN_SPEED	"MIN speed in us"
#define MSG_ACC_DELAY	"Acc delay in us"
#define MSG_RESET 	"Reset EEprom ? "
#define MSG_RAZ 	"Reset values ? "
#define MSG_MOVE	"Move in mm"

#define MSG_VALID 		"Enter:"
#define SIZE_MSG_VALID 		COUNTOF(MSG_VALID)
#define MSG_CHOICE 		"Y/N"
#define SIZE_MSG_CHOICE		COUNTOF(MSG_CHOICE)
#define MSG_DIRECTION 		"</>/N"
#define SIZE_MSG_DIRECTION	COUNTOF(MSG_DIRECTION)

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

  uint8_t block[8] = {
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11111
  };

  // Public functions -----------------------------------------------------------
public:
  Display();
  ~Display();

  const void begin();
  const void version();
  const void clear();
  const void blank(uint8_t size);
  const void blinkValue(uint8_t _index, const char value[], int _arraySize, bool _blank, uint8_t offset);
  const void loadBar();

  // Render menu item
  const void renderIconOn(uint8_t pos, bool currentItem);
  const void renderItem(const char item[]);
  const void renderIconChild();

protected :
  // MenuSetting
  const void enginePrintHome(char label[], char arrayValue[]);
  const void enginePrintFillChar(int8_t last, int8_t index, uint8_t buffSize, const char arrayValue[], uint8_t offset);
  const void enginePrintEditMode(bool setMode);
  const void enginePrintSave(double value);
  const void enginePrintResetConfirm(bool razValues);
  const void enginePrintMoveDirection(double value);

};

#endif /* SRC_DISPLAY_H_ */
