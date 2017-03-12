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

  Wire.setClock(400000); // change i2c speed to 400Khz

  _lcd.createChar(ICONBLOCK[0], block);
  _lcd.createChar(ICONLEFT[0], left);
  _lcd.createChar(ICONRIGHT[0], right);
  _lcd.createChar(ICONSTOP[0], stop);
  _lcd.createChar(ICONRESUME[0], resume);
  _lcd.createChar(ICONBACK[0], back);


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


/******************************************************************************
 * brief   : Reprint char if cursor move.
 * details : If cursor is moving on new position we need to print the
 * character of the old index and a block on new character.
 ******************************************************************************/
const void Display::blinkSelection(uint8_t index, char actionBar[], uint8_t sizeAB,
				   uint8_t positionAB, uint8_t wordSize, bool editMode)
{
  static bool basculeSet = true;

  if(basculeSet) // Block ON
    {
      if(editMode) // refresh only number
	{
	  _lcd.setCursor(index, positionAB);
	  _lcd.print(actionBar[index]);
	}
      else // refresh action bar
	{
	  _lcd.setCursor(sizeAB, positionAB);
	  for(uint8_t i=sizeAB; i<LCD_CHARS; i++){_lcd.write(actionBar[i]);}
	}
      basculeSet = false;
    }
  else // Block OFF
    {
      if(wordSize > 1) // Word
	{
	  _lcd.setCursor(index, positionAB);
	  blank(wordSize);
	}
      else // Number or single character
	{
	  _lcd.setCursor(index, positionAB);
	  editMode ? _lcd.write(' ') : _lcd.write(ICONBLOCK);
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
      _lcd.write(ICONBLOCK);
      delay(10);
    }
}


// Render menu item ------------------------------------------------------------
const void Display::renderIconOn(uint8_t pos, bool currentItem)
{
  _lcd.setCursor(0, pos);
  currentItem ? _lcd.write(ICONRIGHT) : _lcd.write(20);
}

const void Display::renderItem(const char item[])
{
  _lcd.print(item);
}

const void Display::renderIconChild()
{
  _lcd.write(20);
  _lcd.write(ICONRIGHT);
  blank(LCD_CHARS);
}

// Engine menu setting ----------------------------------------------------------
const void Display::engineFillChar(int8_t last, int8_t index, uint8_t buffSize,
				   const char arrayValue[], uint8_t offset)
{
  if(index>last || index<last ) // Forward , backward
    {
      _lcd.setCursor((last + offset), LCD_LINES);

      (index<last && index == buffSize-2) ?
	  _lcd.write(ICONRIGHT) : _lcd.print(arrayValue[last]);
    }
}


/******************************************************************************
 * brief   : Print "edit" message.
 * details : When you click on number we switch on edit mode.
 ******************************************************************************/
const void Display::engineEditMode( uint8_t positionAB)
{
  _lcd.setCursor(MAX_SIZE_VALUE, positionAB);

  for(uint8_t i=0; i<LCD_CHARS; i++) _lcd.write(' ');

  _lcd.setCursor((LCD_CHARS-SIZE_MSG_EDIT+1), positionAB);
  _lcd.print(MSG_EDIT);
}


/******************************************************************************
 * brief   : Displays the backup message
 * details : When you click on "save"
 ******************************************************************************/
const void Display::engineSave(float value, char unit[], char actionBar[], uint8_t positionAB)
{
  _lcd.clear();
  _lcd.setCursor(0,0);
  _lcd.print(MSG_SAVE); _lcd.print(value); _lcd.print(unit);

  _lcd.setCursor(0,positionAB);
  _lcd.print(actionBar);
}


const void Display::engineWindingValue(float coilLength, float wireSize, uint32_t coilTurns, uint32_t currentTurns)
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


/******************************************************************************
 * brief   : Print all informations to set current value.
 * Exemple :
 * ------------------  ------------------
 * |WireSize in mm  |  |label           |
 * |0.00      Save/>|  |     actionBar  | <- position is position of action bar.
 * ------------------  ------------------
 ******************************************************************************/
const void Display::engineSetValue(char label[], char actionBar[], uint8_t positionAB)
{
  _lcd.clear();
  _lcd.setCursor(0, 0);
  _lcd.print(label);
  _lcd.setCursor(0, positionAB);
  _lcd.print(actionBar);
}

/******************************************************************************
 * brief   : Message for moving menu.
 ******************************************************************************/
const void Display::engineMoving(float value, char unit[], bool dir)
{
  _lcd.clear();
  _lcd.setCursor(0, 0);
  _lcd.print(value); _lcd.print(unit); _lcd.print(" in ");
  dir ? _lcd.print(MSG_CLOCK) : _lcd.print(MSG_C_CLOCK);
  _lcd.setCursor(0, LCD_LINES);
  _lcd.print(MSG_FOR_STOP);
}


const void Display::engineNewWinding(uint16_t coilTurns)
{
  _lcd.setCursor(0, LCD_LINES);
  _lcd.print(coilTurns);  _lcd.print(UNIT_TR);
}

const void Display::engineResumeWinding(uint16_t coilTurns, uint16_t counter)
{
  _lcd.setCursor(0, LCD_LINES);
  _lcd.print(coilTurns),_lcd.print("/"),_lcd.print(counter), _lcd.print(UNIT_TR);
}

const void Display::engineAjustSpeed(bool refresh, int8_t percent)
{
  if(!refresh)
    {
      _lcd.clear();
      _lcd.setCursor(0,0);
      _lcd.print(MSG_SPEED);
    }

  _lcd.setCursor(0,LCD_LINES);
  _lcd.print(percent); _lcd.print("%"); _lcd.print("  ");
}


const void Display::engineSuspend(char actionBar[], uint8_t positionAB, uint16_t coilTurns, uint16_t counter)
{
  _lcd.clear();
  _lcd.setCursor(0,positionAB);
  _lcd.print(actionBar);
  _lcd.setCursor(0, LCD_LINES);
  _lcd.print(coilTurns),_lcd.print("/"),_lcd.print(counter), _lcd.print(UNIT_TR);
}


const void Display::engineSaveCurrent(char actionBar[], uint8_t positionAB, uint16_t coilTurns, uint16_t counter)
{
  _lcd.clear();
  _lcd.setCursor(0,0);
  _lcd.print(MSG_SAVE_CURRENT);
  _lcd.setCursor(0,positionAB);
  _lcd.print(actionBar);
  _lcd.setCursor(0,positionAB);
  _lcd.print(coilTurns),_lcd.print("/"),_lcd.print(counter), _lcd.print(UNIT_TR);
}


const void Display::windingTurns(uint32_t coilTurns, uint32_t counter)
{
  _lcd.setCursor(0,1);
  _lcd.print("Tr:"), _lcd.print(coilTurns),_lcd.print("/"),_lcd.print(counter), _lcd.print(" ");
}


const void Display::windingSetSpeed(uint16_t speed)
{
  _lcd.clear();
  _lcd.setCursor(0,0);
  _lcd.print(speed);
}
