/*
 * MenuSettings.cpp
 *
 *  Created on: 2 déc. 2016
 *      Author: jo
 */
#include "MenuSettings.h"

const unsigned long delayTimeBlock = 250;
const unsigned long delayTimeBlank = 150;

Setting::Setting(LiquidCrystal_I2C *p_lcd, ClickEncoder *p_Encoder) : _idValue(0),
    _buffSize(0), p_floatingValue(0), p_arrayValue(0)
{
  _lcd = p_lcd;
  _Encoder = p_Encoder;
};
Setting::~Setting(){};


void Setting::getId(const uint8_t id)
{
  _idValue = id;

#ifdef DEBUG
  Serial.print("id : "); Serial.println(_idValue);
  Serial.println("---------------------");
#endif

  affectValues();
}



/* PRIVATE -------------------------------------------------------------------*/
void Setting::affectValues()
{
  switch (_idValue)
  {
#ifdef DEBUG
    Serial.println("set value function : ");
    Serial.println("---------------------");
#endif
    case id_WIRESIZE :
      {
	Serial.print("buffer origin : "); Serial.println(_buff_WireSize);

	strcpy(_label, MSG_WIRE_SIZE);
	p_arrayValue = _buff_WireSize;
	p_floatingValue = &WireSize;
	_buffSize = BUFFSIZE_WIRE;
	break;
      }
    case id_COILLENGTH :
      {
	Serial.print("buffer origin : "); Serial.println(_buff_CoilLength);

	strcpy(_label, MSG_COIL_LENGTH);
	p_arrayValue = _buff_CoilLength;
	p_floatingValue = &CoilLength;
	_buffSize = BUFFSIZE_COIL;
	break;
      }
    case id_TURNS :
      {
	Serial.print("buffer origin : "); Serial.println(_buff_Turns);

	strcpy(_label, MSG_TURNS);
	p_arrayValue =_buff_Turns;
	p_floatingValue = &Turns;
	_buffSize = BUFFSIZE_TURNS;
	break;
      }
    case id_MAX_SPEED :
      {
	strcpy(_label, MSG_MAX_SPEED);
	p_arrayValue =_buff_MaxSpeed;
	p_floatingValue = &MaxSpeed;
	_buffSize = BUFFSIZE_MAX_SPEED;
	break;
      }
    case id_MIN_SPEED :
      {
	strcpy(_label, MSG_MIN_SPEED);
	p_arrayValue =_buff_MinSpeed;
	p_floatingValue = &MinSpeed;
	_buffSize = BUFFSIZE_MIN_SPEED;
	break;
      }
    case id_ACC_DELAY :
      {
	strcpy(_label, MSG_ACC_DELAY);
	p_arrayValue =_buff_AccDelay;
	p_floatingValue = &AccDelay;
	_buffSize = BUFFSIZE_ACC_DELAY;
	break;
      }
  }
#ifdef DEBUG
  Serial.print("label : "); Serial.println(_label);
  Serial.print("array value : "); Serial.println(p_arrayValue);
  Serial.print("value float : "); Serial.println(*p_floatingValue);
  Serial.print("size : "); Serial.println(_buffSize);
  Serial.println("---------------------");
#endif
  engine();
}

float Setting::engine()
{
  _lcd->clear();
  _lcd->setCursor(0, 0);
  _lcd->print(_label);
  _lcd->setCursor(0, LCD_LINES);
  _lcd->print(p_arrayValue);
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

      clampValue(&index, 0, _buffSize-1);

      index = ignoreChar(index, last, p_arrayValue, _buffSize);

      // Print old character to Fill the hole only if we move
      if(index>last || index<last ) // Forward , backward
	{
	  _lcd->setCursor(last, LCD_LINES);
	  (index<last && index == _buffSize-2) ? _lcd->write((byte)IconValid) : _lcd->print(p_arrayValue[last]);
	  // Determine the direction of the next movement.
	  last = index;
	}

      // Blinking of the selected character
      if(timer(currentTime, &lastTime, delayTimeBlock))
	{
	  blinkValue(index, p_arrayValue, _buffSize, false);
	}

      ClickEncoder::Button buttonState = _Encoder->getButton();
      if( buttonState == ClickEncoder::Clicked )
	{
	  // If we want to exit the menu (click on the exit icon)
	  if(index > _buffSize)
	    {
	      setValue();
	      run = false;
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
	      int8_t count = p_arrayValue[index];
	      unsigned long lastTimeSet;

	      while((buttonState = _Encoder->getButton()) != ClickEncoder::Clicked)
		{
		  unsigned long currentTimeSet = millis();

		  count+= _Encoder->getValue();
		  clampValue(&count, '0', '9');

		  p_arrayValue[index] = count;

		  // Blinking value
		  if(timer(currentTimeSet, &lastTimeSet, delayTimeBlank))
		    {
		      blinkValue(index,p_arrayValue, _buffSize, true);
		    }
		}
	      // Refresh screen after set value
	      _lcd->setCursor(LCD_CHARS-(SIZE_MSG_VALID), LCD_LINES);
	      _lcd->print(MSG_VALID);
	      _lcd->write((byte)IconValid);
	    }
	}
      // return the cursor if we are at the end off lcd
      if(index>_buffSize) index = _buffSize-1;
    }
  return false;
}

// Jump to the next index for ignore char.
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

// Blink of the selected character
void Setting::blinkValue(uint8_t _index, char value[], int _arraySize, bool _blank)
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
      _blank ? _lcd->print(" ") : _lcd->write((byte)IconBlock);
      basculeSet = true;
  }
}

void Setting::setValue()
{
  *p_floatingValue = atof(p_arrayValue);
}
