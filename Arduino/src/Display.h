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
#define MSG_MAX_SPEED	"MAX speed"
#define MSG_MIN_SPEED	"MIN speed"
#define MSG_ACC_DELAY	"Acc duration"

#define MSG_VALID 	"Enter:"
#define SIZE_MSG_VALID 	COUNTOF(MSG_VALID)
#define MSG_CHOICE 	"Y/N"
#define SIZE_MSG_CHOICE	COUNTOF(MSG_CHOICE)

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
  void blank(uint8_t size);
  void blinkValue(uint8_t _index, const char value[], int _arraySize, bool _blank, uint8_t offset);

  //Temporaries
  const void reset();

  // Render menu item
  void renderIconOn(uint8_t pos, bool currentItem);
  void renderItem(const char item[]);
  void renderIconChild();

protected :
  // MenuSetting
  void enginePrintHome(char label[], char arrayValue[]);
  void enginePrintFillChar(int8_t last, int8_t index, uint8_t buffSize, const char arrayValue[], uint8_t offset);
  void enginePrintEditMode(bool setMode);
  void enginePrintSave(double value);

private:

};

#endif /* SRC_DISPLAY_H_ */
