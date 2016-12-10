/*
 * Display.cpp
 *
 *  Created on: 5 nov. 2016
 *      Author: jo
 */

#include "Display.h"

Display::Display(LiquidCrystal_I2C *p_lcd)
{
  _lcd = p_lcd;
};

const void Display::begin()
{
  _lcd->begin(LCD_CHARS,LCD_LINES);

  _lcd->createChar(IconLeft, left);
  _lcd->createChar(IconRight, right);
  _lcd->createChar(IconBack, back);
  _lcd->createChar(IconEnter, enter);
  _lcd->createChar(IconValid, valid);
  _lcd->createChar(IconBlock, block);

  // Quick 3 blinks of back-light
  for(uint8_t i = 0; i < 3; i++)
    {
      _lcd->backlight();
      delay(100);
      _lcd->noBacklight();
      delay(100);
    }
  _lcd->backlight();
}

const void Display::version()
{
  _lcd->setCursor(0, 0);
  _lcd->print("Coil Winder v1.0");
  _lcd->setCursor(0,1);
  _lcd->print("click to start");
}
