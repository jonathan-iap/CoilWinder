/*
 * MenuSettings.cpp
 *
 *  Created on: 2 déc. 2016
 *      Author: jo
 */
#include "MenuSettings.h"

const unsigned long delayTime = 150;

Setting::Setting(LiquidCrystal_I2C *p_lcd, ClickEncoder *p_Encoder)
{
  _lcd = p_lcd;
  _Encoder = p_Encoder;
};
Setting::~Setting(){};

float Setting::setValue(char value[], int arraySize, char valueName[])
{
  _lcd->clear();
  _lcd->setCursor(0, 0);
  _lcd->print(valueName);
  _lcd->setCursor(0, LCD_LINES);
  _lcd->print(value);
  _lcd->setCursor(LCD_CHARS-(SIZE_MSG_VALID),LCD_LINES);
  _lcd->print(MSG_VALID);
  _lcd->write((byte)IconValid);

  bool run = true;
  int8_t last = 0;
  int8_t index = 0;
  unsigned long lastTime;

  while(run)
    {
      unsigned long currentTime = millis();

      index += _Encoder->getValue();

      clampValue(&index, 0, arraySize-1);

      index = ignoreChar(index, last, value, arraySize);

      // Print old character to Fill the hole only if we move
      if(index>last || index<last ) // Forward , backward
	{
	  _lcd->setCursor(last, LCD_LINES);
	  (index<last && index == arraySize-2) ? _lcd->write((byte)IconValid) : _lcd->print(value[last]);
	  // Determine the direction of the next movement.
	  last = index;
	}

      // Blinking of the selected character
      if(timer(currentTime, &lastTime, delayTime))
	{
	  blinkValue(index, value, arraySize);
	}

      ClickEncoder::Button buttonState = _Encoder->getButton();
      if( buttonState == ClickEncoder::Clicked )
	{
	  // If we want to exit the menu (click on the exit icon)
	  if(index > arraySize)
	    {
	      return atof(value);
	    }
	  // Else set the new value
	  else
	    {
	      // Erase the exit icon to show we are in the edit mode
	      _lcd->setCursor(LCD_CHARS-(SIZE_MSG_VALID), LCD_LINES);
	      for(uint8_t i = 0; i<SIZE_MSG_VALID; i++)
		{
		  _lcd->print(' ');
		}
	      _lcd->setCursor(LCD_CHARS-3, LCD_LINES);
	      _lcd->print("set");

	      // Set Value
	      int8_t count = value[index];
	      unsigned long lastTimeSet;

	      while((buttonState = _Encoder->getButton()) != ClickEncoder::Clicked)
		{
		  unsigned long currentTimeSet = millis();

		  count+= _Encoder->getValue();
		  clampValue(&count, '0', '9');

		  value[index] = count;

		  // Blinking value
		  if(timer(currentTimeSet, &lastTimeSet, delayTime))
		    {
		      blinkValue(index,value, arraySize);
		    }
		}
	      // Refresh screen after set value
	      _lcd->setCursor(LCD_CHARS-(SIZE_MSG_VALID), LCD_LINES);
	      _lcd->print(MSG_VALID);
	      _lcd->write((byte)IconValid);
	    }
	}
      // return the cursor if we are at the end off lcd
      if(index>arraySize) index = arraySize-1;
    }
  return false;
}

uint8_t Setting::ignoreChar(uint8_t _index, uint8_t _last, char value[], int _arraySize)
{
  if(_index > _arraySize-2)
    {
      _index = LCD_CHARS-1;
    }
  else
    {
      if((value[_index] == '.' || value[_index] == '/') && _index>_last)
	{
	  _index ++;
	}
      else if((value[_index] == '.' || value[_index] == '/') && _index <_last)
	{
	  _index --;
	}
    }
  return _index;
}

void Setting::blinkValue(uint8_t _index, char value[], int _arraySize)
{
  static bool basculeSet = true;

  if(basculeSet)
    {
      if(_index > _arraySize)
	{
	  _lcd->setCursor(_index, LCD_LINES);
	  _lcd->write((byte)IconValid);
	}
      else
	{
	  _lcd->setCursor(_index, LCD_LINES);
	  _lcd->print(value[_index]);
	}
      basculeSet = false;
    }
  else{
      _lcd->setCursor(_index, LCD_LINES);
      _lcd->print(" ");
      basculeSet = true;
  }
}
