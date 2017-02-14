/*
 * MenuSettings.cpp
 *
 *  Created on: 2 déc. 2016
 *      Author: jo
 */
#include "MenuSettings.h"

Setting::Setting(ClickEncoder *p_Encoder, Display *p_Display, Coil *p_Coil)
:  speedPercent(100),
   _buffSize(0),
   _idValue(0),
   p_arrayValue(0),
   _sizeBuffValue(0),
   p_floatingValue(0),
   _positionAB(0),
   _index(0),
   _minIndex(0),
   _tmpId(0)
{
  _Encoder = p_Encoder;
  _Display = p_Display;
  _Coil = p_Coil;
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


void Setting::getFloatingValue()
{
  *p_floatingValue = atof(p_arrayValue);
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
	      if(save) saveValue_old(*p_floatingValue);
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


// Move cursor on character to select.
void Setting::selectCharacter(int8_t *index, int8_t *last, const char arrayValue[],
			      uint8_t buffSize, uint8_t offset, bool cursoJumpEnd)
{
  static unsigned long lastTime;
  unsigned long currentTime = millis();

  *index += _Encoder->getValue();

  clampValue(index, 0, buffSize-1);

  *index = IgnoreChar(*index, *last, arrayValue, buffSize, cursoJumpEnd);

  _Display->engineFillChar(*last, *index, buffSize, arrayValue, offset);

  // Blinking of the selected character
  if(timer(currentTime, &lastTime, DelayTimeBlock))
    {
      _Display->blinkValue(*index, arrayValue, buffSize, false, offset);
    }

  // To determine the direction of the next movement.
  *last = *index;
}

void Setting::selectCharacter(int8_t *index, int8_t *last)
{
  static unsigned long lastTime;
  unsigned long currentTime = millis();

  *index += _Encoder->getValue();

  clampValue(index, 1, 4);

  // Print old world if we move : Forward , backward
  if(*index>*last || *index<*last )
    {
      _Display->windingSelectAction();
    }
  // Blinking of the selected world
  if(timer(currentTime, &lastTime, DelayTimeBlock))
    {
      _Display->blinkWorld(*index);
    }

  // To determine the direction of the next movement.
  *last = *index;
}

// Jump to the next index to ignore char.
int8_t Setting::IgnoreChar(int8_t index, int8_t last, const char value[],
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
      if(timer(currentTimeSet, &lastTimeSet, DelayTimeBlank))
	{
	  _Display->blinkValue(index, arrayValue, buffSize, true, 0);
	}
    }
  // Refresh screen after set value
  _Display->engineEditMode(false);
}


// Print current value and ask save? if yes save in eeprom memory.
void Setting::saveValue_old(float value)
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


uint8_t Setting::menuSuspend()
{
  int8_t currentIndex = 0;
  int8_t lastIndex = 0;
  uint8_t returnValue = 0;
  bool isRun = true;

  _Display->clear();
  _Display->windingSelectAction();
  _Display->windingTurns(_Coil->getTurns(), _Coil->getCurrentTurns());

  while(isRun)
    {
      selectCharacter(&currentIndex, &lastIndex);

      ClickEncoder::Button buttonState = _Encoder->getButton();
      if( buttonState == ClickEncoder::Clicked )
	{
	  switch (currentIndex)
	  {
	    case 1 : {returnValue = SET_CURRENT_SPEED; break;}//speed
	    case 2 : {returnValue = EXIT_WINDING; break;}//exit
	    case 3 : {returnValue = SAVE; break;}//save
	    case 4 : {returnValue = CONTINUE_WINDING; break;}//back
	  }

	  isRun = EXIT;
	}
    }
  return returnValue;
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
      // Set direction by clicking on "</>" or "N" to exit.
      selectCharacter(&currentIndex, &lastIndex, MSG_DIRECTION, (SIZE_MSG_DIRECTION-1),
		      (LCD_CHARS-SIZE_MSG_DIRECTION+1), false);

      ClickEncoder::Button buttonState = _Encoder->getButton();
      if( buttonState == ClickEncoder::Clicked )
	{
	  if(currentIndex < 4) // Select direction "</>"
	    {
	      currentIndex == 0 ? direction = C_CLOCK :direction = CLOCK;

	      _Coil->setSpeed(AccDelay,MaxSpeed, MinSpeed, MaxSpeed);
	      _Coil->runOnlyCarriage(direction, tmp_distance);
	      _Coil->disableMotors();
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
	  if(currentIndex < 4) // Select direction "</>"
	    {
	      currentIndex == 0 ? direction = C_CLOCK :direction = CLOCK;

	      // set and start displacement.
	      _Coil->setSpeed(AccDelay,MaxSpeed, MinSpeed, MaxSpeed);
	      _Coil->runOnlyCoil(direction, tmp_turns);
	      _Coil->disableMotors();
	    }
	  run = EXIT;
	}
    }
}


uint16_t Setting::ajustSpeed(bool initSpeed, int8_t *speedInPercent)
{
  bool refresh = true;
  bool run = true;
  int8_t oldSpeed = 0;

  _Display->engineAjustSpeed( !refresh, initSpeed, *speedInPercent);

  while(run)
    {
      // Value increase when you turn encoder
      *speedInPercent += _Encoder->getValue();
      // Clamp to 1% at 100%
      clampValue(speedInPercent, 1, 100);
      // Refresh LCD only if value change
      if(*speedInPercent != oldSpeed) _Display->engineAjustSpeed(refresh, !initSpeed, *speedInPercent);

      oldSpeed = *speedInPercent;

      // Click for exit
      ClickEncoder::Button buttonState = _Encoder->getButton();
      if( buttonState == ClickEncoder::Clicked ) run = EXIT;

    }

  return map(*speedInPercent, 0, 100, MinSpeed, MaxSpeed);
}


void Setting::runWinding(bool resumeCurrent, bool resumeSaved)
{
  bool start_MSG = true;
  bool isRun = true;

  // Pass all values for winding.
  _Coil->setWinding(CoilLength, WireSize, Turns);
  _Coil->setSpeed(AccDelay,MaxSpeed, MinSpeed, ajustSpeed(start_MSG, &speedPercent));
  if(resumeSaved)
    {
      _idValue = id_RESUME;
      read(0, _idValue);
      _Coil->setSteps(TotalSteps, LayerSteps);
    }

  while(isRun)
    {
      // Start winding with "runMultiLayer()"
      if(_Coil->runMultiLayer(resumeCurrent, resumeSaved)) // "runMultiLayer()" return true if winding is finished.
	{
	  _Coil->disableMotors();
	  isRun = false;
	}
      // If user click on encoder during winding.
      else
	{
	  menuSuspendStart:
	  uint8_t state = menuSuspend();

	  switch (state)
	  {
	    case SET_CURRENT_SPEED :
	      {
		_Coil->updateSpeed(ajustSpeed(false, &speedPercent));
		break;
	      }
	    case EXIT_WINDING :
	      {
		isRun = false;
		break;
	      }
	    case SAVE :
	      {
		_idValue = id_RESUME;
		TotalSteps = _Coil->getTotalStepsCounter();
		LayerSteps = _Coil->getLayerStepsCounter();
		saveValue_old(0);
		goto menuSuspendStart;
	      }
	    case CONTINUE_WINDING : {break;}
	  }
	}
    }
}


/***********************************************************************************
DEV
 ***********************************************************************************/

/******************************************************************************
 * brief   : For edit values
 * details : Drive all functions to set value in edit mode.
 ******************************************************************************/
void Setting::actionMenu(const uint8_t id)
{
  _idValue = id;
  setValueFromId();
  navigationEngine();
}

void Setting::actionMenu(const uint8_t id, const char label[], char arrayValue[],
			 const uint8_t sizeOfArrayValue, float *value, const char unit[],
			 const char actionBar[], const uint8_t sizeActionBar, uint8_t AB_LinePosition)
{
  _idValue = id;
  _tmpId = id; // todo delete this
  setValues(label, arrayValue,  sizeOfArrayValue, value, unit,  actionBar,
	    sizeActionBar,  AB_LinePosition);
  navigationEngine();
}


/******************************************************************************
 * brief   : Assign value and action
 * details : Depending the Id, we set value and action if needed.
 ******************************************************************************/
void Setting::setValueFromId()
{
  switch (_idValue)
  {
    case id_WIRESIZE :
      {
	setValues(MSG_WIRE_SIZE, _buff_WireSize, BUFFSIZE_WIRE, &WireSize, UNIT_MM,
		  ACTIONBAR_SETVALUE, SIZE_AB_SETVALUE, LCD_LINES);
	break;
      }
    case id_COILLENGTH :
      {
	setValues(MSG_COIL_LENGTH, _buff_CoilLength, BUFFSIZE_COIL, &CoilLength, UNIT_MM,
		  ACTIONBAR_SETVALUE, SIZE_AB_SETVALUE, LCD_LINES);
	break;
      }
    case id_TURNS :
      {
	setValues(MSG_TURNS, _buff_Turns, BUFFSIZE_TURNS, &Turns, UNIT_TR,
		  ACTIONBAR_SETVALUE, SIZE_AB_SETVALUE, LCD_LINES);
	break;
      }
    case id_MAX_SPEED :
      {
	setValues(MSG_MAX_SPEED, _buff_MaxSpeed, BUFFSIZE_MAX_SPEED, &MaxSpeed, UNIT_US,
		  ACTIONBAR_SETVALUE, SIZE_AB_SETVALUE, LCD_LINES);
	break;
      }
    case id_MIN_SPEED :
      {
	setValues(MSG_MIN_SPEED, _buff_MinSpeed, BUFFSIZE_MIN_SPEED, &MinSpeed, UNIT_US,
		  ACTIONBAR_SETVALUE, SIZE_AB_SETVALUE, LCD_LINES);
	break;
      }
    case id_ACC_DELAY :
      {
	setValues(MSG_ACC_DELAY, _buff_AccDelay, BUFFSIZE_ACC_DELAY, &AccDelay, UNIT_US,
		  ACTIONBAR_SETVALUE, SIZE_AB_SETVALUE, LCD_LINES);
	break;
      }
    case id_RESET :
      {
	setValues(MSG_RESET, id_RESET, ACTIONBAR_CHOICE, SIZE_AB_CHOICE, LCD_LINES);
	break;
      }
    case id_RAZ :
      {
	setValues(MSG_RAZ, id_RAZ, ACTIONBAR_CHOICE, SIZE_AB_CHOICE, LCD_LINES);
	break;
      }
  }
}


/******************************************************************************
 * brief   : Set the values to work with them
 * details : Assign variable from memory and depending what we want to set.
 * Value was display like this :
 *        ----------------------
 * LCD -> |label               |
 *        |value        btn/btn|
 *        ----------------------
 *******************************************************************************/
void Setting::setValues(const char label[], char arrayValue[], const uint8_t sizeOfArrayValue,
			float *value, const char unit[], const char actionBar[],
			const uint8_t sizeActionBar, uint8_t AB_LinePosition)
{
  // Displayed on left top.
  strcpy(_label, label);
  // Pass values saved from memory.
  p_arrayValue 		= arrayValue;
  _sizeBuffValue	= sizeOfArrayValue;
  p_floatingValue 	= value;
  strcpy(_unit, unit);
  // Action bar
  setActionBar(arrayValue, sizeOfArrayValue, actionBar, sizeActionBar, AB_LinePosition);
  _index = 0;

#ifdef DEBUG
  Serial.println("***********************");
  Serial.print("_label          : "); Serial.println(_label);
  Serial.print("p_arrayValue    : "); Serial.println(p_arrayValue);
  Serial.print("p_floatingValue : "); Serial.println(*p_floatingValue);
  Serial.print("_sizeBuffValue  : "); Serial.println(_sizeBuffValue);
  Serial.print("_unit           : "); Serial.println(_unit);
  Serial.print("_actionBar      : "); Serial.println(_actionBar);
  Serial.println("***********************");
#endif
}


void Setting::setValues(const char label[], uint8_t tmpId, const char actionBar[],
			const uint8_t sizeActionBar, uint8_t AB_LinePosition)
{
  strcpy(_label, label);
  _tmpId = tmpId;
  // Action bar
  setActionBar(0, 0, actionBar, sizeActionBar, AB_LinePosition);
  _index = 0;
}


/******************************************************************************
 * brief   : Prepare action bar.
 * details : If "sizeBuffValue>0", action bar is concatenated with string
 * "label" and string "action bar" with a middle space.
 * This action bar is displayed and manipulated on a single line.
 * return  : New string is stored in "actionBar"
 ******************************************************************************/
void Setting::setActionBar(char arrayValue[], const uint8_t sizeOfArrayValue,
			   const char actionBar[], const uint8_t sizeActionBar,
			   uint8_t AB_LinePosition)
{
  uint8_t offset = LCD_CHARS - sizeActionBar;

  arrayValue == 0 ? _minIndex = offset : _minIndex = 0;

  // Fill the action bar
  for (uint8_t i=0; i<LCD_CHARS; i++)
    {
      if (i < (sizeOfArrayValue-1)) _actionBar[i] = *arrayValue++;
      else if ( i <= offset) _actionBar[i] = ' ';
      else _actionBar[i] = *actionBar++;
    }

  _actionBar[LCD_CHARS+1]= 0; // End of array (null character)

  _positionAB = AB_LinePosition; // Display position
}


/******************************************************************************
 * brief   : Engine to navigate and edit current menu.
 * details : Enslave click encoder to navigate into edit menu.
 ******************************************************************************/
void Setting::navigationEngine()
{
  bool run = true;

  int8_t  lastIndex = 0;
  uint8_t lastSense = 0;
  uint8_t wordSize = 0;
  unsigned long lastTime;

  _Display->engine_setValue(_label, _actionBar, _positionAB);

  while(run)
    {
      cursorMovement(&lastIndex, &lastSense, &wordSize, &lastTime);

      ClickEncoder::Button buttonState = _Encoder->getButton();
      if( buttonState == ClickEncoder::Clicked )
	{
	  run = selectedAction(wordSize);
	}
    }
}


/******************************************************************************
 * brief   : Move cursor.
 * details : Manages the displacement of cursor and as well as being displayed.
 ******************************************************************************/
void Setting::cursorMovement(int8_t *lastIndex, uint8_t *lastSense,
			     uint8_t *wordSize, unsigned long *lastTime)
{
  unsigned long currentTime = millis();

  // Get encoder movement, clamp returned value and detect sense of motion
  _index += _Encoder->getValue();

  clampValue(&_index, _minIndex, (LCD_CHARS-1));

  uint8_t motion = motionSense(_index, *lastIndex);

  // Index of cursor is moved if a movement has been detected
  if(motion > 0)
    {
      // Index is jumped to the end of the word +1
      if(*wordSize > 1 && motion == CURSOR_MOVE_RIGHT)
	{
	  _index+=*wordSize;
	}

      ignoreChar(_actionBar, &_index, motion);

      *wordSize = wordDetect(_actionBar, &_index, motion);

      *lastSense = motion; // Old the motion sense
    }


  // Blinking of the selected character
  if(timer(currentTime, lastTime, DelayTimeBlock) || motion>0 )
    {
      _Display->blinkSelection(_index, _actionBar, *wordSize, false);
    }

  // To determine the direction of the next movement.
  *lastIndex = _index;
}


/******************************************************************************
 * brief   : Set value selected
 * details : Allows you to set the selected value (between 0 to 9).
 * Click for exit.
 ******************************************************************************/
void Setting::editValue(int8_t index)
{
  // Erase icons or words to show we are in the edit mode
  _Display->engineEditMode();

  // Set the value as long as the user does not click
  int8_t count = _actionBar[index];
  unsigned long lastTimeSet;

  ClickEncoder::Button btnState = _Encoder->getButton();
  while((btnState = _Encoder->getButton()) != ClickEncoder::Clicked)
    {
      unsigned long currentTimeSet = millis();

      count+= _Encoder->getValue();
      clampValue(&count, '0', '9');

      _actionBar[index] = count;

      // Blinking value
      if(timer(currentTimeSet, &lastTimeSet, DelayTimeBlank))
	{
	  _Display->blinkSelection(index, _actionBar, 0, true);
	}
    }
}


/******************************************************************************
 * brief   : Sorts the selected action
 * details : Depending where the cursor is placed, it's identified which action
 * to be taken. Work with call backs.
 ******************************************************************************/
bool Setting::selectedAction(uint8_t wordSize)
{
  char tmp_word[wordSize+1] = {0};

  Serial.print("_tmpId: "); Serial.println(_tmpId);

  // Numbers
  if(isNumber(_actionBar, _index))
    {
      editValue(_index);
      return CONTINU;
    }

  // Words
  else if(isWord(_actionBar, _index, wordSize, tmp_word))
    {
      if(buffercmp((char*)KEYWORD_SAVE, tmp_word, SIZE_KEYWORD_SAVE))
	{
	  setSave(); // Display and update values
	  return CONTINU;
	}
      else if(buffercmp((char*)KEYWORD_YES, tmp_word, SIZE_KEYWORD_YES))
	{
	  switch (_tmpId)
	  {
	    case id_SAVE : {saveCurrent(); return EXIT; break;}
	    case id_RESET: {resetAll(); return EXIT; break;}
	    case id_RAZ  : {RAZ_All(); return EXIT; break;}
	  }
	}
      else if(buffercmp((char*)KEYWORD_NO, tmp_word, SIZE_KEYWORD_NO))
	{
	  switch (_tmpId)
	  {
	    case id_SAVE : {retry(); return CONTINU; break;}
	    case id_RESET || id_RAZ : {return EXIT; break;}
	  }
	}
      else if(buffercmp((char*)KEYWORD_SPEED, tmp_word, SIZE_KEYWORD_SPEED))
	return EXIT;
      else if(buffercmp((char*)KEYWORD_EXIT, tmp_word, SIZE_KEYWORD_EXIT))
	return EXIT;
      else return 0;
    }

  // Icons
  else
    {
      switch (_actionBar[_index])
      {
	case ICONLEFT[0] :
	{
	  if(_tmpId == id_MOVE_CARRIAGE || _tmpId == id_MOVE_COIL){
	      moving(C_CLOCK); return EXIT;
	  }
	  break;
	}
	case ICONRIGHT[0] :
	{
	  if(_tmpId == id_MOVE_CARRIAGE || _tmpId == id_MOVE_COIL){
	      moving(CLOCK); return EXIT;
	  }
	  else{
	      update(); return EXIT;
	  }
	  break;
	}
      }
    }
  return 0;
}


/*CallBacks __________________________________________________________________*/


/******************************************************************************
 * brief   : Update "p_arrayValue" and "p_floatingValue"
 * details : Copy the value from "_actionBar" into "p_arrayValue" and
 * update "p_floatingValue".
 ******************************************************************************/
void Setting::update()
{
  bufferCopy(_actionBar, p_arrayValue, 0, _sizeBuffValue);
  *p_floatingValue = atof(p_arrayValue);
}


/******************************************************************************
 * brief   : Restore display to re-edit value.
 ******************************************************************************/
void Setting::retry()
{
  setValueFromId();
  _Display->engine_setValue(_label, _actionBar, _positionAB);
}

/******************************************************************************
 * brief   : Sub menu save
 * details : Print current value and ask save? if yes save in eeprom memory.
 ******************************************************************************/
void Setting::setSave()
{
  _tmpId = id_SAVE;
  _index = 0; // Need when action bar is changed

  if(_idValue != id_RESUME)
    {
      update();
      setActionBar(0, 0, ACTIONBAR_CHOICE, SIZE_AB_CHOICE, LCD_LINES);
      _Display->engineSave(*p_floatingValue, _unit,_actionBar, _positionAB);
    }
}


/******************************************************************************
 * brief   : Save current value.
 * details : Store current value in EEprom memory.
 ******************************************************************************/
void Setting::saveCurrent()
{
  save(p_arrayValue, _idValue);
  _Display->loadBar();
}


/******************************************************************************
 * brief   : Reset all value in EEprom memories.
 * details : All value are reset with the value contain in the
 * "Configuration.h" file. But not updated, just reseted!
 ******************************************************************************/
void Setting::resetAll()
{
  reset();
  _Display->loadBar();
}


/******************************************************************************
 * brief   : Update all values from the EEprom memories.
 * details : Restore all values stored in memory. Also after a reset,
 * user have choice to continue is work with own values or update all values.
 ******************************************************************************/
void Setting::RAZ_All()
{
  readAll();
  _Display->loadBar();
}


void Setting::moving(bool direction)
{
  update();

  // Message displacement.
  _Display->engineMoving(*p_floatingValue, _unit, direction);

  // set and start displacement.
  _Coil->setSpeed(AccDelay,MaxSpeed, MinSpeed, MaxSpeed);

  if(_tmpId == id_MOVE_CARRIAGE) { _Coil->runOnlyCarriage(direction, *p_floatingValue);}
  else {_Coil->runOnlyCoil(direction, *p_floatingValue);}

  _Coil->disableMotors();
}
