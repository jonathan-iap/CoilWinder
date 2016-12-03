/*
 * Navigation.cpp
 *
 *  Created on: 2 déc. 2016
 *      Author: jo
 */
#include "Arduino.h"
#include "string.h"
#include "Configuration.h"
#include "Navigation.h"
#include "Display.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "ClickEncoder.h"

extern LiquidCrystal_I2C lcd;
extern ClickEncoder Encoder;

const unsigned long delayTime = 150;

char valueToSet[LCD_CHARS+1] = "\0";
char s_save[]="Save?Y/N";

// format the character string to "00.00  Save?Y/N"
const void initString(uint8_t _valueSize, bool _decimals)
{
  // Array "valueToSet[]" is empty
  if(valueToSet[0] == '\0')
    {
      char decimals[]=".00";
      uint8_t blank = LCD_CHARS - sizeof(s_save) - _valueSize;
      char tmp_blank[blank+1]="\0";
      char tmp_value[_valueSize+2]="\0"; // +2 is for end of line and decimal point character

      if(_decimals)
	{
	  for(uint8_t i=0; i<(sizeof(tmp_value)-4); i++)
	    {
	      tmp_value[i] = '0';
	      tmp_value[i+1] = '\0';
	    }
	  strcat(tmp_value, decimals);
	}
      else
	{
	  for(uint8_t i=0; i<=(sizeof(tmp_value)-2); i++)
	    {
	      i != (sizeof(tmp_value)-2) ? tmp_value[i] = '0': tmp_value[i] = ' ';
	    }
	}

      for(uint8_t i=0; i<blank; i++)
	{
	  tmp_blank[i] = ' ';
	}
      // End of line character
      tmp_value[sizeof(tmp_value)-1] = '\0';
      tmp_blank[sizeof(tmp_blank)-1] = '\0';
      // Concatenation of string : "00.00   Save?Y/N" or "0000   Save?Y/N"
      strcat(valueToSet, tmp_value);
      strcat(valueToSet, tmp_blank);
      strcat(valueToSet, s_save);
    }
}

// clamp value on a positive interval
const uint8_t clampValue(int8_t _value, uint8_t _min, uint8_t _max)
{
  if(_value > _max){ _value = _max; return _value;}
  else if (_value < _min){ _value = _min; return _value;}
  else return _value;
}

const uint8_t ignoreChar(int8_t _index, int8_t _last)
{
  static int8_t blankIndex = -1;

  if((valueToSet[_index] == '.' || valueToSet[_index] == ' ' || valueToSet[_index] == '/')
      && _index>_last)
    {
      //Jump forward
      if(valueToSet[_index] == ' ')
	{
	  if(blankIndex == -1){ blankIndex = _index; } // init indexBlank if it the first time

	  while(valueToSet[_index] != 'Y'){ _index++; }
	}
      else{ _index ++; }
    }
  else if((valueToSet[_index] == '.' || valueToSet[_index] == '?' || valueToSet[_index] == '/')
      && _index<_last)
    {
      //Jump backward
      valueToSet[_index] == '?' ? _index = blankIndex-1 : _index --;
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

const void blinkValue(uint8_t _index)
{
  static bool basculeSet = true;
  if(basculeSet)
    {
      lcd.setCursor(_index, LCD_LINES);
      lcd.print(valueToSet[_index]);
      basculeSet = false;
    }
  else{
      lcd.setCursor(_index, LCD_LINES);
      lcd.print(" ");
      basculeSet = true;
  }
}


Navigation::Navigation(){};
Navigation::~Navigation(){};

void Navigation::getValue()
{

}

void Navigation::setValue()
{
  lcd.clear();
  initString(4,true);
  lcd.setCursor(0, LCD_LINES);
  lcd.print(valueToSet);

  bool stay = true;
  int8_t last = 0;
  int8_t index = 0;
  unsigned long lastTime;

  while(stay)
    {
      unsigned long currentTime = millis();

      index += Encoder.getValue();

      index = clampValue(index, 0, LCD_CHARS-1);
      // Ignore character
      index = ignoreChar(index, last);

      // debug
      lcd.setCursor(0,0);
      lcd.print(index);
      lcd.print("   ");
      lcd.print(last);
      lcd.print("   ");

      // Print old character to Fill the hole only if we move
      if(index>last) // Forward
	{
	  lcd.setCursor(last, LCD_LINES);
	  lcd.print(valueToSet[last]);
	}
      if(index<last)  // Backward
	{
	  lcd.setCursor(last,LCD_LINES);
	  lcd.print(valueToSet[last]);
	}

      // Blinking of the selected character
      if(timer(currentTime, &lastTime, delayTime))
	{
	  blinkValue(index);
	}

      // To determine the direction of the next movement.
      last = index;

      ClickEncoder::Button buttonState = Encoder.getButton();
      if( buttonState == ClickEncoder::Clicked )
	{
	  // If we want to exit the menu (click on the exit icon)
	  if(valueToSet[index] == 'Y')
	    {
	      lcd.setCursor(LCD_CHARS-5,0);
	      lcd.print(atof(valueToSet));
	      stay = false; // exit
	    }
	  else if(valueToSet[index] == 'N')
	    {
	      lcd.setCursor(LCD_CHARS-5,0);
	      lcd.print("     ");
	      stay = false; // exit
	    }
	  // Else set the new value
	  else
	    {
	      // Erase the exit icon to show you are in the edit mode
	      lcd.setCursor(LCD_CHARS-(sizeof(s_save)-1), LCD_LINES);
	      for(uint8_t i = 0; i<(sizeof(s_save)); i++)
		{
		  lcd.print(' ');
		}
	      lcd.setCursor(LCD_CHARS-3, LCD_LINES);
	      lcd.print("set");

	      uint8_t count = valueToSet[index];
	      unsigned long lastTimeSet;
	      // Set Value
	      while((buttonState = Encoder.getButton()) != ClickEncoder::Clicked)
		{
		  unsigned long currentTimeSet = millis();

		  count+= Encoder.getValue();
		  count = clampValue(count,'0','9');

		  valueToSet[index] = count;

		  // Blinking value
		  if(timer(currentTimeSet, &lastTimeSet, delayTime))
		    {
		      blinkValue(index);
		    }
		}
	      // Exit by printing the string save.
	      lcd.setCursor(LCD_CHARS-(sizeof(s_save)-1), LCD_LINES);
	      lcd.print(s_save);
	    }
	}
    }
}

