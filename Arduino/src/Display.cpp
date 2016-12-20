/*
 * Display.cpp
 *
 *  Created on: 5 nov. 2016
 *      Author: jo
 */

#include "Display.h"

LiquidCrystal_I2C _lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

Display::Display(){}
Display::~Display(){}

const void Display::begin()
{
  _lcd.begin(LCD_CHARS,LCD_LINES);

  _lcd.createChar(IconLeft, left);
  _lcd.createChar(IconRight, right);
  _lcd.createChar(IconBack, back);
  _lcd.createChar(IconEnter, enter);
  _lcd.createChar(IconValid, valid);
  _lcd.createChar(IconBlock, block);

  // Quick 3 blinks of back-light
  for(uint8_t i = 0; i < 3; i++)
    {
      _lcd.backlight();
      delay(100);
      _lcd.noBacklight();
      delay(100);
    }
  _lcd.backlight();
}

const void Display::version()
{
  _lcd.setCursor(0, 0);
  _lcd.print("Coil Winder v1.0");
  _lcd.setCursor(0,1);
  _lcd.print("click to start");
}

const void Display::clear()
{
  _lcd.clear();
}

void Display::blank(uint8_t size)
{
  for(int i=0; i<size; i++) _lcd.print(" ");
}

// blink value on current cursor
void Display::blinkValue(uint8_t _index, const char value[], int _arraySize, bool _blank, uint8_t offset)
{
  static bool basculeSet = true;

  if(basculeSet)
    {
      if(_index > _arraySize)
	{
	  _lcd.setCursor((_index+offset), LCD_LINES);
	  _lcd.write((byte)IconValid);
	}
      else
	{
	  _lcd.setCursor((_index+offset), LCD_LINES);
	  _lcd.print(value[_index]);
	}
      basculeSet = false;
    }
  else{
      _lcd.setCursor((_index+offset), LCD_LINES);
      _blank ? _lcd.print(" ") : _lcd.write((byte)IconBlock);
      basculeSet = true;
  }
}


// Temporaries
const void Display::reset()
{
  _lcd.clear();
  _lcd.setCursor(0,0);
  _lcd.print("EEprom memory");
  _lcd.setCursor(0,1);
  _lcd.print("Reset");
}
// Render menu item ------------------------------------------------------------
void Display::renderIconOn(uint8_t pos, bool currentItem)
{
  _lcd.setCursor(0, pos);
  currentItem ? _lcd.write((uint8_t)IconEnter) : _lcd.write(20);
}

void Display::renderItem(const char item[])
{
  _lcd.print(item);
}

void Display::renderIconChild()
{
  _lcd.write(20);
  _lcd.write((uint8_t)IconRight);
}

// Engine menu setting ----------------------------------------------------------
void Display::enginePrintHome(char label[], char arrayValue[])
{
  _lcd.clear();
  _lcd.setCursor(0, 0);
  _lcd.print(label);
  _lcd.setCursor(0, LCD_LINES);
  _lcd.print(arrayValue);
  _lcd.setCursor(LCD_CHARS-(SIZE_MSG_VALID),LCD_LINES);
  _lcd.print(MSG_VALID);
  _lcd.write((byte)IconValid);
}

void Display::enginePrintFillChar(int8_t last, int8_t index, uint8_t buffSize,
				  const char arrayValue[], uint8_t offset)
{
  if(index>last || index<last ) // Forward , backward
    {
      _lcd.setCursor((last + offset), LCD_LINES);
      (index<last && index == buffSize-2) ?
	  _lcd.write((byte)IconValid) : _lcd.print(arrayValue[last]);
    }
}

void Display::enginePrintEditMode(bool setMode)
{
  _lcd.setCursor(LCD_CHARS-(SIZE_MSG_VALID), LCD_LINES);
  if(setMode)
    {
      for(uint8_t i = 0; i<SIZE_MSG_VALID; i++)
	{
	  _lcd.print(' ');
	}
      _lcd.setCursor(LCD_CHARS-3, LCD_LINES);
      _lcd.print("set");
    }
  else
    {
      _lcd.print(MSG_VALID);
      _lcd.write((byte)IconValid);
    }
}

void Display::enginePrintSave(double value)
{
  _lcd.clear();
  _lcd.setCursor(0,0);
  _lcd.print("Save in eeprom ?");
  _lcd.setCursor(0,LCD_LINES);
  _lcd.print(value);
  _lcd.setCursor((LCD_CHARS-SIZE_MSG_CHOICE+1), LCD_LINES);
  _lcd.print(MSG_CHOICE);
}
