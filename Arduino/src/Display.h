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

#define MAX_SIZE_VALUE		6 // max value is -> #define INIT_COIL "000.00"

#define CURSOR_MOVE_LEFT	1
#define CURSOR_MOVE_RIGHT	2

/* Custom characters defined in the first 8 characters of the LCD */
#define ICONBLOCK	"\x01"// Print only as a char. This will have 'unexpected' results when used in a string!
#define ICONLEFT 	"\x02"
#define ICONRIGHT	"\x03"

/* Units ______________________________________________________________*/
#define UNIT_MM		"mm"
#define UNIT_TR		"Tr"
#define UNIT_US		"uS"
/* Labels _____________________________________________________________*/
#define MSG_TEST		"Rework nav"
#define MSG_WIRE_SIZE	"Wire size in" UNIT_MM
#define MSG_COIL_LENGTH	"Length in " UNIT_MM
#define MSG_TURNS	"Turns in " UNIT_TR
#define MSG_MAX_SPEED	"MAX speed in " UNIT_US
#define MSG_MIN_SPEED	"MIN speed in " UNIT_US
#define MSG_ACC_DELAY	"Acc delay in " UNIT_US
#define MSG_RESET 	"Reset EEprom ? "
#define MSG_RAZ 	"Reset values ? "
#define MSG_MOVE	"Move in " UNIT_MM
/* Keywords ___________________________________________________________*/
#define KEYWORD_SAVE		"Save"
#define SIZE_KEYWORD_SAVE 	COUNTOF(KEYWORD_SAVE)
#define KEYWORD_SPEED		"Speed"
#define SIZE_KEYWORD_SPEED 	COUNTOF(KEYWORD_SPEED)
#define KEYWORD_EXIT		"Exit"
#define SIZE_KEYWORD_EXIT 	COUNTOF(KEYWORD_EXIT)
#define KEYWORD_YES		"Yes"
#define SIZE_KEYWORD_YES 	COUNTOF(KEYWORD_YES)
#define KEYWORD_NO		"No"
#define SIZE_KEYWORD_NO 	COUNTOF(KEYWORD_NO)
/* Actions bar ________________________________________________________*/
#define ACTIONBAR_SETVALUE	KEYWORD_SAVE "/" ICONRIGHT
#define SIZE_AB_SETVALUE	COUNTOF(ACTIONBAR_SETVALUE)
#define ACTIONBAR_SAVE		KEYWORD_YES "/" KEYWORD_NO
#define ACTIONBAR_MOVE		ICONLEFT "/" ICONRIGHT " " KEYWORD_EXIT
/* Recognition of words _______________________________________________*/
// Just number
#define isNUMBER		10
// Keyword
#define isWORD_SAVE		11
#define isWORD_SPEED		12
#define isWORD_EXIT		13
#define isWORD_YES		14
#define isWORD_NO		15
// Icons
#define isICONLEFT		20
#define isICONRIGHT		21
#define isICONBACK		22

#define MSG_SPEED		"Speed"
#define SIZE_MSG_SPEED 		COUNTOF(MSG_SPEED)
#define MSG_EXIT		"Exit"
#define SIZE_MSG_EXIT 		COUNTOF(MSG_EXIT)
#define CURSOR_EXIT		(((LCD_CHARS-(SIZE_MSG_SAVE+SIZE_MSG_SAVE+SIZE_MSG_EXIT))/2)+SIZE_MSG_SPEED)
#define MSG_SAVE		"Save"
#define SIZE_MSG_SAVE 		COUNTOF(MSG_SAVE)
#define MSG_EDIT		"Edit"
#define SIZE_MSG_EDIT 		COUNTOF(MSG_EDIT)
#define MSG_CHOICE_SAVE		"Save/"
#define SIZE_MSG_CHOICE_SAVE	COUNTOF(MSG_CHOICE_SAVE)
#define MSG_CHOICE 		"Y/N"
#define SIZE_MSG_CHOICE		COUNTOF(MSG_CHOICE)
#define MSG_DIRECTION 		"</>/N"
#define SIZE_MSG_DIRECTION	COUNTOF(MSG_DIRECTION)

class Display
{
  // Public variables -----------------------------------------------------------
public :

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


  // Public functions -----------------------------------------------------------
public:
  Display();
  ~Display();

  const void begin();
  const void version();
  const void clear();
  const void blank(uint8_t size);
  const void blinkValue(uint8_t _index, const char value[], int _arraySize, bool _blank, uint8_t offset);
  const void blinkSelection(uint8_t index, char actionBar[], uint8_t wordSize, bool editMode); // New
  const void blinkWorld(uint8_t index);
  const void loadBar();

  // Render menu item
  const void renderIconOn(uint8_t pos, bool currentItem);
  const void renderItem(const char item[]);
  const void renderIconChild();

  // MenuSetting
  const void engineHome(char label[], char arrayValue[]);
  const void engineFillChar(int8_t last, int8_t index, uint8_t buffSize, const char arrayValue[], uint8_t offset);
  const void engineEditMode(bool setMode);
  const void engineEditMode(); //New
  const void engineSave(float value);
  const void engineSave_N(float value); //New
  const void engineResetConfirm(bool razValues);
  const void engineMoveDirection(float value, bool turns);
  const void engineWindingValue(float coilLength, float wireSize, unsigned long coilTurns, uint16_t currentTurns);
  const void engineAjustSpeed(bool refresh, bool initMSG, int8_t percent);

  // Winding
  const void windingTurns(uint16_t coilTurns,uint16_t counter);
  const void windingSelectAction();
  const void windingSetSpeed(uint16_t speed);

  const void engine_setValue(char label[], char container[], uint8_t position);
  const void TestActionBar();
};

#endif /* SRC_DISPLAY_H_ */
