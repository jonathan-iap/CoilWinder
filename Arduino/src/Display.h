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

#define MAX_SIZE_VALUE  6 // max value is -> #define INIT_COIL "000.00"

#define CURSOR_MOVE_LEFT   1
#define CURSOR_MOVE_RIGHT  2

/* Custom characters defined in the first 8 characters of the LCD */
#define ICONBLOCK   "\x01"// Print only as a char. This will have 'unexpected' results when used in a string!
#define ICONLEFT    "\x02"
#define ICONRIGHT   "\x03"
#define ICONSTOP    "\x04"
#define ICONRESUME  "\x05"
#define ICONBACK    "\x06"

/* Units ______________________________________________________________*/
#define UNIT_MM   "mm"
#define UNIT_TR   "Tr"
#define UNIT_RPM  "RPM"
#define UNIT_US   "us"
#define UNIT_STEP "step"
/* Labels _____________________________________________________________*/
#define MSG_HOMING        "Homing : "
#define MSG_CONFIRM       "Confirm ?"
#define MSG_WIRE_SIZE     "Wire size in " UNIT_MM
#define MSG_COIL_LENGTH   "Length in " UNIT_MM
#define MSG_TURNS         "Turns in " UNIT_TR
#define MSG_MAX_SPEED     "MAX speed in " UNIT_RPM
#define MSG_MIN_SPEED     "MIN speed in " UNIT_RPM
#define MSG_ACC_DELAY     "Acc delay in " UNIT_US
#define MSG_ACC_INCR      "Acc incr in " UNIT_STEP
#define MSG_EDIT          "Edit"
#define SIZE_MSG_EDIT     COUNTOF(MSG_EDIT)
#define MSG_SAVE          "Save? "
#define MSG_NEW_COIL      "Start new Coil ?"
#define MSG_RESUME        "Resume current ?"
#define MSG_RESUME_SAVED  "Resume saved ?"
#define MSG_RESET         "Reset EEprom ? "
#define MSG_RAZ           "Update values ? "
#define MSG_MOVE          "Move in " UNIT_MM
#define MSG_CLOCK         "Clk  "
#define MSG_C_CLOCK       "CClk "
#define MSG_FOR_STOP      "Click to stop"
#define MSG_SPEED         "Winding speed :"
#define MSG_SAVE_CURRENT  "Save session ?"
#define MSG_VALUE_TO_HIGH "Value to high !"
#define MSG_CURRENT_SPEED "Speed: "
#define MSG_EMPTY       " "
/* Keywords ___________________________________________________________*/
#define KEYWORD_MOVE        "Move"
#define SIZE_KEYWORD_MOVE   COUNTOF(KEYWORD_MOVE)
#define KEYWORD_SET         "Set"
#define SIZE_KEYWORD_SET    COUNTOF(KEYWORD_SET)
#define KEYWORD_SAVE        "Save"
#define SIZE_KEYWORD_SAVE   COUNTOF(KEYWORD_SAVE)
#define KEYWORD_SPEED       "Speed"
#define SIZE_KEYWORD_SPEED  COUNTOF(KEYWORD_SPEED)
#define KEYWORD_EXIT        "Exit"
#define SIZE_KEYWORD_EXIT   COUNTOF(KEYWORD_EXIT)
#define KEYWORD_NEXT        "Next"
#define SIZE_KEYWORD_NEXT   COUNTOF(KEYWORD_NEXT)
#define KEYWORD_BACK        "Back"
#define SIZE_KEYWORD_BACK   COUNTOF(KEYWORD_BACK)
#define KEYWORD_YES         "Yes"
#define SIZE_KEYWORD_YES    COUNTOF(KEYWORD_YES)
#define KEYWORD_NO          "No"
#define SIZE_KEYWORD_NO     COUNTOF(KEYWORD_NO)
/* Actions bar ________________________________________________________*/
#define ACTIONBAR_SETHOME        KEYWORD_MOVE "/" KEYWORD_SET "|" KEYWORD_EXIT
#define SIZE_AB_SETHOME          COUNTOF(ACTIONBAR_SETHOME)
#define ACTIONBAR_SETVALUE       KEYWORD_SAVE "/" ICONRIGHT
#define SIZE_AB_SETVALUE         COUNTOF(ACTIONBAR_SETVALUE)
#define ACTIONBAR_CHOICE         KEYWORD_YES "/" KEYWORD_NO
#define SIZE_AB_CHOICE           COUNTOF(ACTIONBAR_CHOICE)
#define ACTIONBAR_CHOICE_SIMPLE  "Y/N"
#define SIZE_AB_CHOICE_SIMPLE    COUNTOF(ACTIONBAR_CHOICE_SIMPLE)
#define ACTIONBAR_SENSE_SAVE     ICONLEFT "/" ICONRIGHT "|" KEYWORD_SAVE "/" KEYWORD_EXIT
#define SIZE_AB_SENSE_SAVE       COUNTOF(ACTIONBAR_SENSE_SAVE)
#define ACTIONBAR_MOVE           ICONLEFT "/" ICONRIGHT "|" KEYWORD_EXIT
#define SIZE_AB_MOVE             COUNTOF(ACTIONBAR_MOVE)
#define ACTIONBAR_MOVE_BACK      ICONLEFT "/" ICONRIGHT "|" KEYWORD_BACK
#define SIZE_AB_MOVE_BACK        COUNTOF(ACTIONBAR_MOVE_BACK)
#define ACTIONBAR_MOVE_NEXT      ICONLEFT "/" ICONRIGHT "|" KEYWORD_NEXT
#define SIZE_AB_MOVE_NEXT        COUNTOF(ACTIONBAR_MOVE_NEXT)
#define ACTIONBAR_SUSPEND        KEYWORD_SPEED "/" KEYWORD_SAVE "/" ICONSTOP "/" ICONRESUME
#define SIZE_AB_SUSPEND          COUNTOF(ACTIONBAR_SUSPEND)


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

  uint8_t stop[8] = {
      0b00000,
      0b00000,
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b11111,
      0b00000
  };

  uint8_t resume[8] = {
      0b00000,
      0b00000,
      0b10100,
      0b10110,
      0b10111,
      0b10110,
      0b10100,
      0b00000
  };

  byte back[8] = {
      0b00111,
      0b00001,
      0b00001,
      0b00101,
      0b01101,
      0b11111,
      0b01100,
      0b00100
  };

  // Public functions -----------------------------------------------------------
public:
  Display();
  ~Display();

  const void begin();
  const void version();
  const void clear();
  const void blank(uint8_t size);
  const void blinkSelection(uint8_t index, char actionBar[], uint8_t sizeAB,
			    uint8_t positionAB, uint8_t wordSize, bool editMode);
  const void loadBar();

  // Render menu item
  const void renderIconOn(uint8_t pos, bool currentItem);
  const void renderItem(const char item[]);
  const void renderIconChild();

  // MenuSetting
  const void engineFillChar(int8_t last, int8_t index, uint8_t buffSize, const char arrayValue[], uint8_t offset);
  const void engineEditMode(uint8_t positionAB);
  const void engineSave(float value, char unit[], char actionBar[], uint8_t positionAB);
  const void engineSave(bool dir, char actionBar[], uint8_t positionAB);
  const void engineWindingValue(float coilLength, float wireSize, uint32_t coilTurns, uint32_t currentTurns);
  const void engineAjustSpeed(bool refresh, bool initMSG, int8_t percent);
  const void engineSetValue(char label[], char actionBar[], uint8_t positionAB);
  const void engineSense(bool dir);
  const void engineNewWinding(uint16_t coilTurns);
  const void engineResumeWinding(uint16_t coilTurns, uint16_t counter);
  const void engineAjustSpeed(bool refresh, int8_t percent);
  const void engineSuspend(char actionBar[], uint8_t positionAB, uint16_t coilTurns,uint16_t counter);
  const void engineSaveCurrent(char actionBar[], uint8_t positionAB, uint16_t coilTurns,uint16_t counter);
  const void engineValueLimit();
  // Winding
  const void windingGetTurns(uint16_t target, uint16_t counter);
  const void windingGetSpeedPercent(uint16_t speed);
  const void windingGetDisplacement(float target, float counter);

  // Debug
  const void print(uint8_t x, uint8_t y, uint32_t value);
};

#endif /* SRC_DISPLAY_H_ */
