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

#define SAVE		true
#define NOT_SAVE	false
#define delayTimeBlock 	250
#define delayTimeBlank 	150

class Setting: public Memory, public Coil
{
public :

  Setting(ClickEncoder *p_Encoder, Display *p_Display);
  ~Setting();

  void getId(const uint8_t id);
  void resetAction(bool razValues);
  void moveValue();

private:

  ClickEncoder *_Encoder;
  Display *_Display;

  uint8_t _idValue;
  uint8_t _buffSize;
  float *p_floatingValue;
  char *p_arrayValue;
  char _label[17]; // Minimum lcd screen 16 + 1 for null character

  void affectValues(const char label[], char arrayValue[],uint8_t buffSize ,float *value);
  void idToValue();
  void engine(bool save);
  void selectCharacter(int8_t *index, int8_t *last, const char arrayValue[] ,
		       uint8_t buffSize, uint8_t offset, bool cursoJumpEnd);
  int8_t ignoreChar(int8_t index, int8_t last, const char value[], int arraySize,
		    bool jumpEnd);
  void editValue(char arrayValue[], uint8_t buffSize, int8_t index,
		 ClickEncoder::Button buttonState);
  void setValue();
  void saveValue(float value);
};

#endif /* SRC_MENUSETTINGS_H_ */
