/*
 * Display.cpp
 *
 *  Created on: 5 nov. 2016
 *      Author: jo
 */

#include "Display.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"

extern LiquidCrystal_I2C lcd;

/*_____ CONSTRUCTOR _____ */

Display::Display()
{}

Display::~Display()
{}


const void Display::home()
{
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
