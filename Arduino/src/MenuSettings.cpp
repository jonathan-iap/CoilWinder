/*
 * MenuSettings.cpp
 *
 *  Created on: 2 déc. 2016
 *      Author: jo
 */
#include "MenuSettings.h"
#include "Arduino.h"
#include "string.h"
#include "Configuration.h"
#include "Display.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "ClickEncoder.h"

extern LiquidCrystal_I2C lcd;
extern ClickEncoder Encoder;

const unsigned long delayTime = 150;

const char s_valid[] = "Valid:";
const char s_save[] = "Save?Y/N";

#define s_valid_SIZE sizeof(s_valid)
#define s_save_SIZE sizeof(s_save)

// clamp value on a positive interval
void clampValue(int8_t *_val, uint8_t _min, uint8_t _max)
{
  if(*_val > _max){ *_val = _max;}
  else if (*_val < _min){ *_val = _min;}
  else;
}

const uint8_t ignoreChar(uint8_t _index, uint8_t _last, char value[], int _arraySize)
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

const bool timer(unsigned long currentTime, unsigned long *oldTime, unsigned long delay)
{
  if (currentTime - *oldTime >= delay)
    {
      *oldTime = currentTime;
      return true;
    }
  return false;
}

const void blinkValue(uint8_t _index, char value[], int _arraySize)
{
  static bool basculeSet = true;

  if(basculeSet)
    {
      if(_index > _arraySize)
	{
	  lcd.setCursor(_index, LCD_LINES);
	  lcd.write((byte)IconValid);
	}
      else
	{
	  lcd.setCursor(_index, LCD_LINES);
	  lcd.print(value[_index]);
	}
      basculeSet = false;
    }
  else{
      lcd.setCursor(_index, LCD_LINES);
      lcd.print(" ");
      basculeSet = true;
  }
}



Setting::Setting(){};
Setting::~Setting(){};

void Setting::setValue(char value[], int arraySize)
{

  lcd.clear();
  lcd.setCursor(0, LCD_LINES);
  lcd.print(value);
  lcd.setCursor(LCD_CHARS-(s_valid_SIZE),LCD_LINES);
  lcd.print(s_valid);
  lcd.write((byte)IconValid);

  bool run = true;
  int8_t last = 0;
  int8_t index = 0;
  unsigned long lastTime;

  while(run)
    {
      //debug
      start:

      unsigned long currentTime = millis();

      index += Encoder.getValue();

      clampValue(&index, 0, arraySize-1);

      index = ignoreChar(index, last, value, arraySize);

      // debug
      lcd.setCursor(0,0);
      lcd.print(index);
      lcd.print(" ");
      //delay(200);
      lcd.print(last);
      lcd.print(" ");

      // Print old character to Fill the hole only if we move
      if(index>last || index<last ) // Forward , backward
	{
	  lcd.setCursor(last, LCD_LINES);
	  (index<last && index == arraySize-2) ? lcd.write((byte)IconValid) : lcd.print(value[last]);
	  // Determine the direction of the next movement.
	  last = index;
	}

      // Blinking of the selected character
      if(timer(currentTime, &lastTime, delayTime))
	{
	  blinkValue(index, value, arraySize);
	}

      ClickEncoder::Button buttonState = Encoder.getButton();
      if( buttonState == ClickEncoder::Clicked )
	{
	  // If we want to exit the menu (click on the exit icon)
	  if(index > arraySize)
	    {
	      lcd.setCursor(LCD_CHARS-9,0);
	      lcd.print(atof(value));
	      //debug
	      goto start;
	      run = false; // exit
	    }
	  // Else set the new value
	  else
	    {
	      // Erase the exit icon to show we are in the edit mode
	      lcd.setCursor(LCD_CHARS-(s_valid_SIZE), LCD_LINES);
	      for(uint8_t i = 0; i<s_valid_SIZE; i++)
		{
		  lcd.print(' ');
		}
	      lcd.setCursor(LCD_CHARS-3, LCD_LINES);
	      lcd.print("set");

	      int8_t count = value[index];
	      unsigned long lastTimeSet;
	      // Set Value
	      while((buttonState = Encoder.getButton()) != ClickEncoder::Clicked)
		{
		  unsigned long currentTimeSet = millis();

		  count+= Encoder.getValue();
		  clampValue(&count, '0', '9');

		  value[index] = count;

		  // Blinking value
		  if(timer(currentTimeSet, &lastTimeSet, delayTime))
		    {
		      blinkValue(index,value, arraySize);
		    }
		}
	      // Refresh screen after set value
	      lcd.setCursor(LCD_CHARS-(s_valid_SIZE), LCD_LINES);
	      lcd.print(s_valid);
	      lcd.write((byte)IconValid);
	    }
	}
      // return the cursor if we are at the end off lcd
      if(index>arraySize) index = arraySize-1;
    }
}

