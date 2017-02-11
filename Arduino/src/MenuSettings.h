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


#define NOT_SAVE		false
#define SAVE			true
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
  void runWinding(bool resumeCurrent, bool resumeSaved);


private:

  ClickEncoder *_Encoder;
  Display *_Display;
  Coil *_Coil;

  int8_t speedPercent;
  uint8_t _buffSize;

  void affectValues(const char label[], char arrayValue[],uint8_t buffSize ,float *value);
  void idToValue();
  void engine(bool save);
  void selectCharacter(int8_t *index, int8_t *last, const char arrayValue[] ,
		       uint8_t buffSize, uint8_t offset, bool cursoJumpEnd);
  void selectCharacter(int8_t *index, int8_t *last);
  int8_t IgnoreChar(int8_t index, int8_t last, const char value[], int arraySize,
		    bool jumpEnd);
  void editValue(char arrayValue[], uint8_t buffSize, int8_t index,
		 ClickEncoder::Button buttonState);
  void saveValue(float value);
  uint16_t ajustSpeed(bool initSpeed, int8_t *speedInPercent);
  uint8_t menuSuspend();

  /*Dev----------------------------------------------------------------*/
  uint8_t _idValue;
  char _label[16]; // Minimum lcd screen 16 + 1 for null character
  char *p_arrayValue;
  uint8_t _sizeBuffValue;
  float *p_floatingValue;
  const char *p_arrayAB;
  uint8_t _sizeAB;
  uint8_t _positionAB; // Where the action bar will be displayed
  uint8_t _minIndex;
  char _actionBar[LCD_CHARS]={0};

public:
  void editionMenu(const uint8_t id);

private:
  void setValueFromId();
  void setValues(const char label[], char arrayValue[], const uint8_t sizeLabelVal,
		 float *value, const char labelBtn[], const uint8_t sizeLabelBtn);
  void setActionBar(char arrayValue[], char labelAB[]);
  void setAll(const char label[], char arrayValue[], const uint8_t sizeOfArrayValue,
	      float *value, const char actionBar[], const uint8_t sizeActionBar,
	      uint8_t AB_position);
  uint8_t navigationEngine();
  void cursorMovement(int8_t *index, int8_t *lastIndex, uint8_t *lastSense,
		      uint8_t *wordSize, unsigned long *lastTime);
  uint8_t selectedAction(int8_t index, uint8_t wordSize);
  void editValue(int8_t index, ClickEncoder::Button buttonState);
  void getFloatingValue();
  bool saveValue(int8_t *index, int8_t *lastIndex, uint8_t *lastSense,
		 uint8_t *wordSize, unsigned long *lastTime);

  /*End Dev----------------------------------------------------------------*/
};

#endif /* SRC_MENUSETTINGS_H_ */
