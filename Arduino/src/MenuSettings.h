/*
 * MenuSettings.h
 *
 *  Created on: 2 déc. 2016
 *      Author: jo
 */

#ifndef SRC_MENUSETTINGS_H_
#define SRC_MENUSETTINGS_H_

#include "Arduino.h"
#include "string.h"
#include "Configuration.h"
#include "Display.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "ClickEncoder.h"
#include "Function.h"
#include "Save.h"

class Setting: public Memory
{
public :

  Setting(LiquidCrystal_I2C *p_lcd, ClickEncoder *p_Encoder);
  ~Setting();

  void getId(const uint8_t id);

private:

  LiquidCrystal_I2C *_lcd;
  ClickEncoder *_Encoder;

  uint8_t _idValue;
  uint8_t _buffSize;
  double *p_floatingValue;
  char *p_arrayValue;
  char _label[17]; // Minimum lcd screen 16 + 1 for null character

  void affectValues();
  float engine();
  uint8_t ignoreChar(uint8_t _index, uint8_t _last, char value[], int _arraySize);
  void blinkValue(uint8_t _index, char value[], int _arraySize, bool _blank);
  void setValue();

};

#endif /* SRC_MENUSETTINGS_H_ */
