/*
 * Display.cpp
 *
 *  Created on: 5 nov. 2016
 *      Author: jo
 */

#include "Display.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

/*_____ CONSTRUCTOR _____ */

Display::Display()
{}

Display::~Display()
{}


const void Display::begin()
{
  lcd.begin(16,2);

  // ------- Quick 3 blinks of back-light  -------------
  for(int i = 0; i< 3; i++)
    {
      lcd.backlight();
      delay(100);
      lcd.noBacklight();
      delay(100);
    }
  lcd.backlight();

  lcd.setCursor(5,0);
  lcd.print("Homing");
}
