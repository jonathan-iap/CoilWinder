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

class Setting
{
public :

  Setting(LiquidCrystal_I2C *p_lcd, ClickEncoder *p_Encoder);
  ~Setting();

  float setValue(char value[], int arraySize, char valueName[]);

private:

  LiquidCrystal_I2C *_lcd;
  ClickEncoder *_Encoder;

  uint8_t ignoreChar(uint8_t _index, uint8_t _last, char value[], int _arraySize);
  void blinkValue(uint8_t _index, char value[], int _arraySize, bool _blank);
};

#endif /* SRC_MENUSETTINGS_H_ */
