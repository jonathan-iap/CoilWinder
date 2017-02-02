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

// Initialize special characters and do a little start blink animation
const void Display::begin()
{
  _lcd.begin(LCD_CHARS,LCD_LINES);

  _lcd.createChar(IconLeft, left);
  _lcd.createChar(IconRight, right);
  _lcd.createChar(IconBack, back);
  _lcd.createChar(IconEnter, enter);
  _lcd.createChar(IconBlock, block);

  // Quick 3 blinks of back-light
  for(uint8_t i=3; i>0; i--)
    {
      _lcd.backlight();
      delay(100);
      _lcd.noBacklight();
      delay(100);
    }
  _lcd.backlight();
}

// Current version
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

// Print variable size blank
const void Display::blank(uint8_t size)
{
  for(int i=size; i>0; i--) _lcd.print(" ");
}

// blink value on current cursor
const void Display::blinkValue(uint8_t _index, const char value[], int _arraySize, bool _blank, uint8_t offset)
{
  static bool basculeSet = true;

  if(basculeSet)
    {
      if(_index > _arraySize)
	{
	  _lcd.setCursor((_index+offset), LCD_LINES);
	  _lcd.write((byte)IconRight);
	}
      else
	{
	  _lcd.setCursor((_index+offset), LCD_LINES);
	  _lcd.print(value[_index]);
	}
      basculeSet = false;
    }
  else
    {
      _lcd.setCursor((_index+offset), LCD_LINES);
      _blank ? _lcd.print(" ") : _lcd.write((byte)IconBlock);
      basculeSet = true;
    }
}

const void Display::blinkWorld(uint8_t index)
{
  static bool basculeSet = true;

  if(basculeSet)
    {
      switch (index)
      {
	case 1 : { _lcd.setCursor(0, 0); blank(SIZE_MSG_SPEED); break; }
	case 2 : { _lcd.setCursor(CURSOR_EXIT, 0); blank(SIZE_MSG_EXIT); break; }
	case 3 : { _lcd.setCursor((LCD_CHARS-SIZE_MSG_SAVE+1), 0); blank(SIZE_MSG_SAVE); break; }
	case 4 : { _lcd.setCursor((LCD_CHARS-1), LCD_LINES); blank(1); break; }
      }
      basculeSet = false;
    }
  else
    {
      switch (index)
      {
	case 1 : { _lcd.setCursor(0, 0); _lcd.print(MSG_SPEED); break; }
	case 2 : { _lcd.setCursor(CURSOR_EXIT, 0); _lcd.print(MSG_EXIT); break; }
	case 3 : { _lcd.setCursor((LCD_CHARS-SIZE_MSG_SAVE+1), 0); _lcd.print(MSG_SAVE); break; }
	case 4 : { _lcd.setCursor((LCD_CHARS-1), LCD_LINES); _lcd.write((byte)IconBack); break; }
      }
      basculeSet = true;
    }
}

// Print an animation load bar
const void Display::loadBar()
{
  _lcd.setCursor(0, LCD_LINES);

  for(uint8_t i=LCD_CHARS; i>0; i--)
    {
      _lcd.write((byte)IconBlock);
      delay(10);
    }
}


// Render menu item ------------------------------------------------------------
const void Display::renderIconOn(uint8_t pos, bool currentItem)
{
  _lcd.setCursor(0, pos);
  currentItem ? _lcd.write((uint8_t)IconEnter) : _lcd.write(20);
}

const void Display::renderItem(const char item[])
{
  _lcd.print(item);
}

const void Display::renderIconChild()
{
  _lcd.write(20);
  _lcd.write((uint8_t)IconRight);
  blank(7);
}

// Engine menu setting ----------------------------------------------------------
const void Display::engineHome(char label[], char arrayValue[])
{
  _lcd.clear();
  _lcd.setCursor(0, 0);
  _lcd.print(label);
  _lcd.setCursor(0, LCD_LINES);
  _lcd.print(arrayValue);
  _lcd.setCursor((LCD_CHARS-SIZE_MSG_CHOICE_SAVE),LCD_LINES);
  _lcd.print(MSG_CHOICE_SAVE); _lcd.write((byte)IconRight);
}

const void Display::engineFillChar(int8_t last, int8_t index, uint8_t buffSize,
				   const char arrayValue[], uint8_t offset)
{
  if(index>last || index<last ) // Forward , backward
    {
      _lcd.setCursor((last + offset), LCD_LINES);

      (index<last && index == buffSize-2) ?
	  _lcd.write((byte)IconRight) : _lcd.print(arrayValue[last]);
    }
}

const void Display::engineEditMode(bool setMode)
{
  _lcd.setCursor((LCD_CHARS-SIZE_MSG_CHOICE_SAVE), LCD_LINES);
  if(setMode)
    {
      for(uint8_t i=SIZE_MSG_CHOICE_SAVE; i>0; i--)
	{
	  _lcd.print(' ');
	}
      _lcd.setCursor((LCD_CHARS-SIZE_MSG_EDIT+1), LCD_LINES);
      _lcd.print(MSG_EDIT);
    }
  else
    {
      _lcd.print(MSG_CHOICE_SAVE); _lcd.write((byte)IconRight);
    }
}


const void Display::engineSave(float value)
{
  _lcd.clear();
  _lcd.setCursor(0,0);
  _lcd.print("Save ?");
  if(value > 0)
    {
      _lcd.setCursor(0,LCD_LINES);
      _lcd.print(value);
    }
  _lcd.setCursor((LCD_CHARS-SIZE_MSG_CHOICE+1), LCD_LINES);
  _lcd.print(MSG_CHOICE);
}

const void Display::engineResetConfirm(bool razValues)
{
  _lcd.clear();
  _lcd.setCursor(0,0);
  razValues ? _lcd.print(MSG_RAZ) : _lcd.print(MSG_RESET);
  _lcd.setCursor(0, LCD_LINES);
  _lcd.print(MSG_CHOICE);
}

const void Display::engineMoveDirection(float value, bool turns)
{
  _lcd.clear();
  _lcd.setCursor(0,0);
  _lcd.print("Direction : ");
  _lcd.setCursor(0,LCD_LINES);
  turns ?(_lcd.print((uint16_t)value),_lcd.print("Tr")) : (_lcd.print(value), _lcd.print("mm"));
  _lcd.setCursor((LCD_CHARS-SIZE_MSG_DIRECTION+1), LCD_LINES);
  _lcd.print(MSG_DIRECTION);
}

const void Display::engineWindingValue(float coilLength, float wireSize, unsigned long coilTurns, uint16_t currentTurns)
{
  _lcd.clear();
  _lcd.setCursor(0,0);
  _lcd.print("L:"), _lcd.print(coilLength);
  _lcd.setCursor(LCD_CHARS-6,0);
  _lcd.print("W:"), _lcd.print(wireSize);
  _lcd.setCursor(0,LCD_LINES);
  _lcd.print("Tr:"), _lcd.print(coilTurns),_lcd.print("/"),_lcd.print(currentTurns),  _lcd.print(" ");
}

const void Display::engineAjustSpeed(bool refresh, bool initMSG, int8_t percent)
{
  if(!refresh)
    {
      _lcd.clear();
      _lcd.setCursor(0,0);
      if(initMSG) _lcd.print("Start speed :");
      else _lcd.print("Current speed :");
    }

  _lcd.setCursor(0,LCD_LINES);
  _lcd.print(percent); _lcd.print("%"); _lcd.print("  ");
}



const void Display::windingTurns(uint16_t coilTurns,uint16_t counter)
{
  _lcd.setCursor(0,1);
  _lcd.print("Tr:"), _lcd.print(coilTurns),_lcd.print("/"),_lcd.print(counter), _lcd.print(" ");
}


const void Display::windingSelectAction()
{
  _lcd.setCursor(0,0);
  _lcd.print(MSG_SPEED);
  _lcd.setCursor(CURSOR_EXIT, 0);
  _lcd.print(MSG_EXIT);
  _lcd.setCursor((LCD_CHARS-SIZE_MSG_SAVE+1), 0);
  _lcd.print(MSG_SAVE);
  _lcd.setCursor((LCD_CHARS-1), LCD_LINES);
  _lcd.write((byte)IconBack);
}


const void Display::windingSetSpeed(uint16_t speed)
{
  _lcd.clear();
  _lcd.setCursor(0,0);
  _lcd.print(speed);
}
