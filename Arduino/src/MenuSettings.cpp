/*
 * MenuSettings.cpp
 *
 *  Created on: 2 déc. 2016
 *      Author: jo
 */
#include "MenuSettings.h"

const unsigned long delayTimeBlock = 250;
const unsigned long delayTimeBlank = 150;

Setting::Setting(ClickEncoder *p_Encoder) : _idValue(0),
    _buffSize(0), p_floatingValue(0), p_arrayValue(0)
{
  _Encoder = p_Encoder;
};
Setting::~Setting(){};

// get id of value that need set
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
// Affect variable and pointer through the id
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

// Navigate menu
float Setting::engine()
{
  enginePrintHome(_label, p_arrayValue);

  bool run = true;
  int8_t last = 0;
  int8_t index = 0;

  while(run)
    {
      selectCharacter(&index, &last, p_arrayValue, _buffSize, 0);

      ClickEncoder::Button buttonState = _Encoder->getButton();
      if( buttonState == ClickEncoder::Clicked )
	{
	  // If we want to exit the menu (click on the exit icon)
	  if(index > _buffSize)
	    {
	      setValue();
	      saveValue(*p_floatingValue);
	      run = EXIT;
	    }
	  // Else set the new value
	  else
	    {
	      editValue(p_arrayValue, _buffSize, index, buttonState);
	    }
	}
      // return the cursor if we are at the end off lcd
      if(index>_buffSize) index = _buffSize-1;
    }
  return false;
}

// Move cursor on character to edit
void Setting::selectCharacter(int8_t *index, int8_t *last, const char arrayValue[],
			      uint8_t buffSize, uint8_t offset)
{
  static unsigned long lastTime;
  unsigned long currentTime = millis();

  *index += _Encoder->getValue();

  clampValue(index, 0, buffSize-1);

  *index = ignoreChar(*index, *last, arrayValue, buffSize);

  enginePrintFillChar(*last, *index, buffSize, arrayValue, offset);

  // Blinking of the selected character
  if(timer(currentTime, &lastTime, delayTimeBlock))
    {
      blinkValue(*index, arrayValue, buffSize, false, offset);
    }

  // To determine the direction of the next movement.
  *last = *index;
}

// Jump to the next index for ignore char.
int8_t Setting::ignoreChar(int8_t index, int8_t last, const char value[],
			   int arraySize)
{
  if(index > 3 && index > arraySize-2)
    {
      index = LCD_CHARS-1;
    }
  else
    {
      if((value[index] == '.' || value[index] == '/') && index>last)
	{
	  index ++;
	}
      else if((value[index] == '.' || value[index] == '/') && index <last)
	{
	  index --;
	}
    }
  return index;
}

void Setting::editValue(char arrayValue[], uint8_t buffSize, int8_t index,
			ClickEncoder::Button buttonState)
{
  // Erase the exit icon to show we are in the edit mode
  enginePrintEditMode(true);

  // Set Value
  int8_t count = arrayValue[index];
  unsigned long lastTimeSet;

  while((buttonState = _Encoder->getButton()) != ClickEncoder::Clicked)
    {
      unsigned long currentTimeSet = millis();

      count+= _Encoder->getValue();
      clampValue(&count, '0', '9');

      arrayValue[index] = count;

      // Blinking value
      if(timer(currentTimeSet, &lastTimeSet, delayTimeBlank))
	{
	  blinkValue(index, arrayValue, buffSize, true, 0);
	}
    }
  // Refresh screen after set value
  enginePrintEditMode(false);
}

// convert value of array on double.
void Setting::setValue()
{
  *p_floatingValue = atof(p_arrayValue);
}

void Setting::saveValue(double value)
{
  int8_t currentIndex, lastIndex;
  bool run = true;

  enginePrintSave(value);

  while(run)
    {
      selectCharacter(&currentIndex, &lastIndex, MSG_CHOICE, (SIZE_MSG_CHOICE-1),
		      (LCD_CHARS-SIZE_MSG_CHOICE+1));

      ClickEncoder::Button buttonState = _Encoder->getButton();
      if( buttonState == ClickEncoder::Clicked )
	{
//	  Serial.print("Index value");
//	  Serial.println(currentIndex);
//	  delay(1000);
	  if(currentIndex == 0)
	    {
//	      Serial.println("Save pass");
//	      Serial.print("p_arrayValue : ");
//	      Serial.println(p_arrayValue);
//	      Serial.print("idvalue : ");
//	      Serial.println(_idValue);
//	      delay(2000);
	      memory.save(p_arrayValue, _idValue);
	    }
	  run = EXIT;
	}
    }
}

