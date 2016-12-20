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

class Setting: public Memory, public Display
{
public :

  Setting(ClickEncoder *p_Encoder);
  ~Setting();

  void getId(const uint8_t id);

private:

  //LiquidCrystal_I2C *_lcd;
  ClickEncoder *_Encoder;

  uint8_t _idValue;
  uint8_t _buffSize;
  double *p_floatingValue;
  char *p_arrayValue;
  char _label[17]; // Minimum lcd screen 16 + 1 for null character

  void affectValues();
  float engine();
  void selectCharacter(int8_t *index, int8_t *last, const char arrayValue[] ,uint8_t buffSize, uint8_t offset);
  int8_t ignoreChar(int8_t index, int8_t last, const char value[], int arraySize);
  void editValue(char arrayValue[], uint8_t buffSize, int8_t index, ClickEncoder::Button buttonState);
  void setValue();
  void saveValue(double value);

};

#endif /* SRC_MENUSETTINGS_H_ */
