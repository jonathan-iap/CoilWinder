/*
 * MenuSettings.h
 *
 *  Created on: 2 déc. 2016
 *      Author: jo
 */

#ifndef SRC_MENUSETTINGS_H_
#define SRC_MENUSETTINGS_H_

#include "Arduino.h"
#include "Configuration.h"
#include "Display.h"
#include "ClickEncoder.h"
#include "Function.h"
#include "Save.h"
#include "Winding.h"
#include "Motor.h"

#define MIN_LCD			16 // Minimum lcd screen 15 + 1 for null character
#define NOT_SAVE		false
#define SAVE			true
#define FIRST_LUNCH		true
#define NEW			true
#define RESUME			false
#define SET_CURRENT_SPEED	2
#define EXIT_WINDING		3
#define CONTINUE_WINDING 	4
#define DelayTimeBlock 	 	250
#define DelayTimeBlank 	 	120


class Setting: public Memory
{
public :

  Setting(ClickEncoder *p_Encoder, Display *p_Display, Coil *p_Coil);
  ~Setting();

  void setId(const uint8_t id);
  void resetAction(bool razValues);
  void moveCarriage();
  void moveCoil();
  void runWinding_old(bool resumeCurrent, bool resumeSaved);


private:

  ClickEncoder *_Encoder;
  Display *_Display;
  Coil *_Coil;

  uint8_t _idValue;
  uint8_t _tmp_id;
  char _label[MIN_LCD];

  char *p_arrayValue;
  uint8_t _sizeBuffValue;
  float *p_floatingValue;
  int8_t _speedPercent;
  char _unit[3];

  char _actionBar[LCD_CHARS+1];
  uint8_t _positionAB; // Where the action bar will be displayed

  int8_t _index;
  uint8_t _minIndex;

  uint32_t _homePosition;

public:

  void actionMenu(const uint8_t id);
  void actionMenu(const uint8_t id, char tmp_buffValue[], float *tmp_valFromBuff);
  bool runWinding(bool isFirstLunch, bool isNewCoil);

private:

  void setValueFromId();
  void setValues(const char label[], char arrayValue[], const uint8_t sizeOfArrayValue,
		 float *value, const char unit[], const char actionBar[],
		 const uint8_t sizeActionBar, uint8_t AB_LinePosition);
  void setValues(const char label[], const uint8_t sizeOfArrayValue,
		 const char unit[], const char actionBar[],
		 const uint8_t sizeActionBar, uint8_t AB_LinePosition);
  void setValues(const char label[], const char actionBar[],
		 const uint8_t sizeActionBar, uint8_t AB_LinePosition);
  void setActionBar(char arrayValue[], const uint8_t sizeOfArrayValue,
		    const char actionBar[], const uint8_t sizeActionBar,
		    uint8_t AB_LinePosition);
  void navigationEngine();
  void displaying();
  void cursorMovement(int8_t *lastIndex, uint8_t *lastSense,
		      uint8_t *wordSize, uint32_t *lastTime);
  void editValue(int8_t index);
  bool selectedAction(uint8_t wordSize);

  // CallBacks _______________________________________________________________
  void update();
  void retry();
  void set_AB_MoveCar();
  void set_AB_Home();
  void set_AB_SetHome();
  void set_AB_Save();
  void setHomePosition();
  void saveCurrent();
  void resetAll();
  void RAZ_All();
  void moving(bool direction);
  void setWinding(bool isfirstLunch);
  void adjustSpeed();
  void set_AB_SuspendMenu();
};

#endif /* SRC_MENUSETTINGS_H_ */
