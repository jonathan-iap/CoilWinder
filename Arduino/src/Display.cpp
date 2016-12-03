/*
 * Display.cpp
 *
 *  Created on: 5 nov. 2016
 *      Author: jo
 */
#include "Configuration.h"
#include "Display.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"

extern LiquidCrystal_I2C lcd;

Display::Display(){};

const void Display::begin()
{
  lcd.begin(LCD_CHARS,LCD_LINES);

  lcd.createChar(IconLeft, left);
  lcd.createChar(IconRight, right);
  lcd.createChar(IconBack, back);
  lcd.createChar(IconEnter, enter);

  // Quick 3 blinks of back-light
  for(uint8_t i = 0; i < 3; i++)
    {
      lcd.backlight();
      delay(100);
      lcd.noBacklight();
      delay(100);
    }
  lcd.backlight();
}

const void Display::version()
{
  lcd.setCursor(0, 0);
  lcd.print("Coil Winder v1.0");
  lcd.setCursor(0,1);
  lcd.print("click to start");
}
