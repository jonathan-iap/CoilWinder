/*
 * MenuSettings.cpp
 *
 *  Created on: 2 déc. 2016
 *      Author: jo
 */
#include "MenuSettings.h"

Setting::Setting(ClickEncoder *p_Encoder, Display *p_Display)
: _idValue(0),
  _buffSize(0),
  p_floatingValue(0),
  p_arrayValue(0)
{
  _Encoder = p_Encoder;
  _Display = p_Display;
}
Setting::~Setting(){}

// get id of value that need set
void Setting::setId(const uint8_t id)
{
  _idValue = id;
  idToValue();
}

void Setting::resetAction(bool razValues)
{
  int8_t currentIndex = 0;
  int8_t  lastIndex=0;
  bool run = true;

  _Display->engineResetConfirm(razValues);

  while(run)
    {
      selectCharacter(&currentIndex, &lastIndex, MSG_CHOICE, (SIZE_MSG_CHOICE-1),0, false);

      ClickEncoder::Button buttonState = _Encoder->getButton();
      if( buttonState == ClickEncoder::Clicked )
	{
	  // If we are in "razValue" menu.
	  if(currentIndex == 0 && !razValues) // if "yes"
	    {
	      reset();
	      _Display->loadBar();
	    }
	  // If we are in "reset" menu.
	  else if(currentIndex == 0 && razValues) // if "yes"
	    {
	      readAll();
	      _Display->loadBar();
	    }
	  run = EXIT;
	}
    }
}

/* PRIVATE -------------------------------------------------------------------*/
void Setting::affectValues(const char label[], char arrayValue[],uint8_t buffSize ,float *value)
{
  strcpy(_label, label);
  p_arrayValue = arrayValue;
  _buffSize = buffSize;
  p_floatingValue = value;
}

// Affect variable and pointer through the id
void Setting::idToValue()
{
  switch (_idValue)
  {
    case id_WIRESIZE :
      {
	affectValues(MSG_WIRE_SIZE, _buff_WireSize, BUFFSIZE_WIRE, &WireSize);
	break;
      }
    case id_COILLENGTH :
      {
	affectValues(MSG_COIL_LENGTH, _buff_CoilLength, BUFFSIZE_COIL, &CoilLength);
	break;
      }
    case id_TURNS :
      {
	affectValues(MSG_TURNS, _buff_Turns, BUFFSIZE_TURNS, &Turns);
	break;
      }
    case id_MAX_SPEED :
      {
	affectValues(MSG_MAX_SPEED, _buff_MaxSpeed, BUFFSIZE_MAX_SPEED, &MaxSpeed);
	break;
      }
    case id_MIN_SPEED :
      {
	affectValues(MSG_MIN_SPEED, _buff_MinSpeed, BUFFSIZE_MIN_SPEED, &MinSpeed);
	break;
      }
    case id_ACC_DELAY :
      {
	affectValues(MSG_ACC_DELAY, _buff_AccDelay, BUFFSIZE_ACC_DELAY, &AccDelay);
	break;
      }
  }
  // Start navigation.
  engine(SAVE);
}

// Navigate menu
void Setting::engine(bool save)
{
  bool run = true;
  int8_t last = 0;
  int8_t index = 0;

  _Display->engineHome(_label, p_arrayValue);

  while(run)
    {
      selectCharacter(&index, &last, p_arrayValue, _buffSize, 0, true);

      ClickEncoder::Button buttonState = _Encoder->getButton();
      if( buttonState == ClickEncoder::Clicked )
	{
	  // If we want to exit the menu (click on the exit icon)
	  if(index > _buffSize)
	    {
	      getFloatingValue();
	      if(save) saveValue(*p_floatingValue);
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
}

// Move cursor on character to edit
void Setting::selectCharacter(int8_t *index, int8_t *last, const char arrayValue[],
			      uint8_t buffSize, uint8_t offset, bool cursoJumpEnd)
{
  static unsigned long lastTime;
  unsigned long currentTime = millis();

  *index += _Encoder->getValue();

  clampValue(index, 0, buffSize-1);

  *index = ignoreChar(*index, *last, arrayValue, buffSize, cursoJumpEnd);

  _Display->engineFillChar(*last, *index, buffSize, arrayValue, offset);

  // Blinking of the selected character
  if(timer(currentTime, &lastTime, delayTimeBlock))
    {
      _Display->blinkValue(*index, arrayValue, buffSize, false, offset);
    }

  // To determine the direction of the next movement.
  *last = *index;
}

// Jump to the next index to ignore char.
int8_t Setting::ignoreChar(int8_t index, int8_t last, const char value[],
			   int arraySize, bool jumpEnd)
{
  if( jumpEnd && index > arraySize-2)
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
  _Display->engineEditMode(true);

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
	  _Display->blinkValue(index, arrayValue, buffSize, true, 0);
	}
    }
  // Refresh screen after set value
  _Display->engineEditMode(false);
}

// convert value of array on float.
void Setting::getFloatingValue()
{
  *p_floatingValue = atof(p_arrayValue);
}

// Print current value and ask save? if yes save in eeprom memory.
void Setting::saveValue(float value)
{
  int8_t currentIndex = 0;
  int8_t lastIndex = 0;
  bool run = true;

  _Display->engineSave(value);

  while(run)
    {
      selectCharacter(&currentIndex, &lastIndex, MSG_CHOICE, (SIZE_MSG_CHOICE-1),
		      (LCD_CHARS-SIZE_MSG_CHOICE+1), false);

      ClickEncoder::Button buttonState = _Encoder->getButton();
      if( buttonState == ClickEncoder::Clicked )
	{
	  if(currentIndex == 0) // if "yes"
	    {
	      save(p_arrayValue, _idValue);
	      _Display->loadBar();
	    }
	  run = EXIT;
	}
    }
}

void Setting::moveCarriage()
{
  char tmp_buffDistance[] = {"00.00"};
  float tmp_distance = 0.00;

  affectValues(MSG_MOVE, tmp_buffDistance, 5, &tmp_distance);
  engine(NOT_SAVE);

  int8_t currentIndex = 0;
  int8_t lastIndex = 0;
  bool run = true;
  bool direction = CLOCK;

  // Print direction white "mm" unit value.
  _Display->engineMoveDirection(tmp_distance, false);

  while(run)
    {
      selectCharacter(&currentIndex, &lastIndex, MSG_DIRECTION, (SIZE_MSG_DIRECTION-1),
		      (LCD_CHARS-SIZE_MSG_DIRECTION+1), false);

      ClickEncoder::Button buttonState = _Encoder->getButton();
      if( buttonState == ClickEncoder::Clicked )
	{
	  if(currentIndex < 4)
	    {
	      currentIndex == 0 ? direction = C_CLOCK :direction = CLOCK;

	      setSpeed(AccDelay,MaxSpeed, MinSpeed);
	      runOnlyCarriage(direction, tmp_distance);
	      disableMotors();
	    }
	  run = EXIT;
	}
    }
}

void Setting::moveCoil()
{
  char tmp_buffTurns[] = {"0000"};
  float tmp_turns = 0.00;

  // Set number of turns, that we want to move.
  affectValues(MSG_TURNS, tmp_buffTurns, 5, &tmp_turns);
  engine(NOT_SAVE);

  int8_t currentIndex = 0;
  int8_t lastIndex = 0;
  bool run = true;
  bool direction = CLOCK;

  // Print direction with "Tr" unit value.
  _Display->engineMoveDirection(tmp_turns, true);

  while(run)
    {
      // Set direction by clicking on "</>" or "N" to exit.
      selectCharacter(&currentIndex, &lastIndex, MSG_DIRECTION, (SIZE_MSG_DIRECTION-1),
		      (LCD_CHARS-SIZE_MSG_DIRECTION+1), false);

      ClickEncoder::Button buttonState = _Encoder->getButton();
      if( buttonState == ClickEncoder::Clicked )
	{
	  if(currentIndex < 4)
	    {
	      currentIndex == 0 ? direction = C_CLOCK :direction = CLOCK;

	      // set and start displacement.
	      setSpeed(AccDelay,MaxSpeed, MinSpeed);
	      runOnlyCoil(direction, tmp_turns);
	      disableMotors();
	    }
	  run = EXIT;
	}
    }
}

void Setting::runWinding()
{
  _Display->engineWindingValue(CoilLength, WireSize, Turns);
  setWinding(CoilLength, WireSize, Turns);
  setSpeed(AccDelay,MaxSpeed, MinSpeed);
  runMultiLayer();
  disableMotors();
}
