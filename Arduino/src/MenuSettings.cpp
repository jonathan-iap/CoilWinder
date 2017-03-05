/*
 * MenuSettings.cpp
 *
 *  Created on: 2 déc. 2016
 *      Author: jo
 */
#include "MenuSettings.h"

Setting::Setting(ClickEncoder *p_Encoder, Display *p_Display, Coil *p_Coil)
:  _idValue(0),
   _tmp_id(0),
   p_arrayValue(0),
   _sizeBuffValue(0),
   p_floatingValue(0),
   _speedPercent(100),
   _positionAB(0),
   _index(0),
   _minIndex(0),
   _homePosition(0)
{
  _Encoder = p_Encoder;
  _Display = p_Display;
  _Coil = p_Coil;
}
Setting::~Setting(){}


/******************************************************************************
 * brief   : For edit values
 * details : Drive all functions to set value in edit mode.
 ******************************************************************************/
void Setting::actionMenu(const uint8_t id)
{
  _idValue = id;
  _tmp_id = id;

  setValueFromId();
  navigationEngine();
}


void Setting::actionMenu(const uint8_t id, char tmp_buffValue[], float *tmp_valFromBuff)
{
  _idValue = id;

  p_arrayValue     = tmp_buffValue;
  p_floatingValue  = tmp_valFromBuff;

  setValueFromId();
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
    case id_HOME :
      {
	setValues(MSG_HOMING, ACTIONBAR_SETHOME, SIZE_AB_SETHOME, LCD_LINES);
	break;
      }
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
    case id_MOVE_CARRIAGE :
      {
	_tmp_id = id_MOVE_CARRIAGE;
	setValues(MSG_MOVE, COUNTOF(INIT_MOV_CAR), UNIT_MM, ACTIONBAR_MOVE, SIZE_AB_MOVE,
		  LCD_LINES);
	break;
      }
    case id_MOVE_COIL :
      {
	_tmp_id = id_MOVE_COIL;
	setValues(MSG_MOVE, COUNTOF(INIT_MOV_COIL), UNIT_TR, ACTIONBAR_MOVE, SIZE_AB_MOVE,
		  LCD_LINES);
	break;
      }
    case id_RESET :
      {
	setValues(MSG_RESET, ACTIONBAR_CHOICE, SIZE_AB_CHOICE, LCD_LINES);
	break;
      }
    case id_RAZ :
      {
	setValues(MSG_RAZ, ACTIONBAR_CHOICE, SIZE_AB_CHOICE, LCD_LINES);
	break;
      }
    case id_NEW :
      {
	setValues(MSG_NEW_COIL, ACTIONBAR_CHOICE, SIZE_AB_CHOICE, LCD_LINES);
	break;
      }
    case id_RESUME :
      {
	setValues(MSG_RESUME, ACTIONBAR_CHOICE, SIZE_AB_CHOICE, LCD_LINES);
	break;
      }
    case id_RESUME_SAVE :
      {
	setValues(MSG_RESUME_SAVED, ACTIONBAR_CHOICE, SIZE_AB_CHOICE, LCD_LINES);
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
  p_arrayValue     = arrayValue;
  _sizeBuffValue   = sizeOfArrayValue;
  p_floatingValue  = value;
  strcpy(_unit, unit);
  // Action bar
  setActionBar(arrayValue, sizeOfArrayValue, actionBar, sizeActionBar, AB_LinePosition);

#ifdef DEBUGoff
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


void Setting::setValues(const char label[], const uint8_t sizeOfArrayValue,
			const char unit[], const char actionBar[],
			const uint8_t sizeActionBar, uint8_t AB_LinePosition)
{
  // Displayed on left top.
  strcpy(_label, label);
  // Pass values saved from memory.
  _sizeBuffValue   = sizeOfArrayValue;
  strcpy(_unit, unit);
  // Action bar
  setActionBar(p_arrayValue, sizeOfArrayValue, actionBar, sizeActionBar, AB_LinePosition);
}


void Setting::setValues(const char label[], const char actionBar[],
			const uint8_t sizeActionBar, uint8_t AB_LinePosition)
{
  strcpy(_label, label);
  // Action bar
  setActionBar(0, 0, actionBar, sizeActionBar, AB_LinePosition);
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
  uint8_t offset = LCD_CHARS - (sizeActionBar-1);

  arrayValue == 0 ? _minIndex = offset : _minIndex = 0;

  // Fill the action bar
  for (uint8_t i=0; i<LCD_CHARS; i++)
    {
      if (i < (sizeOfArrayValue-1)) _actionBar[i] = *arrayValue++;
      else if ( i < offset) _actionBar[i] = ' ';
      else _actionBar[i] = *actionBar++;
    }

  _actionBar[LCD_CHARS+1] = 0; // End of array (null character)

  _index = 0;
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
  uint8_t wordSize  = 0;
  uint32_t lastTime;

  // Display all information related to the id
  displaying();

  while(run)
    {
      // Navigate into action bar
      cursorMovement(&lastIndex, &lastSense, &wordSize, &lastTime);

      if( _Encoder->getButton() == ClickEncoder::Clicked)
	{
	  run = selectedAction(wordSize);
	  // Allow to recognize the first word of new action bar (if changed).
	  lastIndex = 0;
	  lastSense = 0;
	  wordSize  = 0;
	}
    }
}


/******************************************************************************
 * brief   : Display message.
 * details : Display the first message when user enter in menu.
 ******************************************************************************/
void Setting::displaying()
{
  _Display->engineSetValue(_label, _actionBar, _positionAB);

  switch(_idValue)
  {
    case id_NEW         : {_Display->engineNewWinding(Turns); break;}
    case id_RESUME      : {_Display->engineResumeWinding(Turns, _Coil->getCurrentTurns()); break;}
    case id_RESUME_SAVE:
      {
	uint32_t tmp_totalSteps = 0;
	getSavedTotalSteps(&tmp_totalSteps); // Read the value of all steps in eeprom
	_Display->engineResumeWinding(Turns, (tmp_totalSteps/M1_STEPS_PER_TR)); break;
      }
  }
}


/******************************************************************************
 * brief   : Move cursor.
 * details : Manages the displacement of cursor and as well as being displayed.
 ******************************************************************************/
void Setting::cursorMovement(int8_t *lastIndex, uint8_t *lastSense,
			     uint8_t *wordSize, uint32_t *lastTime)
{
  uint32_t currentTime = millis();

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
      _Display->blinkSelection(_index, _actionBar, _minIndex, _positionAB,*wordSize, false);
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
  _Display->engineEditMode(_positionAB);

  // Set the value as long as the user does not click
  int8_t count = _actionBar[index];
  uint32_t lastTimeSet;

  while(_Encoder->getButton() != ClickEncoder::Clicked)
    {
      uint32_t currentTimeSet = millis();

      count+= _Encoder->getValue();
      clampValue(&count, '0', '9');

      _actionBar[index] = count;

      // Blinking value
      if(timer(currentTimeSet, &lastTimeSet, DelayTimeBlank))
	{
	  _Display->blinkSelection(index, _actionBar, _minIndex, _positionAB, 0, true);
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

  // Numbers
  if(isNumber(_actionBar, _index))
    {
      editValue(_index);
      return CONTINU;
    }

  // Words
  else if(isWord(_actionBar, _index, wordSize, tmp_word))
    {
      // MOVE _________________________________________________________________
      if(buffercmp((char*)KEYWORD_MOVE, tmp_word, SIZE_KEYWORD_MOVE))
	{
	  set_AB_MoveCar();
	  return CONTINU;
	}
      // SET __________________________________________________________________
      else if(buffercmp((char*)KEYWORD_SET, tmp_word, SIZE_KEYWORD_SET))
	{
	  set_AB_SetHome();
	  return CONTINU;
	}
      // SAVE _________________________________________________________________
      else if(buffercmp((char*)KEYWORD_SAVE, tmp_word, SIZE_KEYWORD_SAVE))
	{
	  set_AB_Save(); // Set tmp_id, display and update values
	  return CONTINU;
	}
      // YES __________________________________________________________________
      else if(buffercmp((char*)KEYWORD_YES, tmp_word, SIZE_KEYWORD_YES))
	{
	  switch (_tmp_id)
	  {
	    case id_SAVE : {
	      saveCurrent();

	      if(_idValue == id_RESUME){ set_AB_SuspendMenu(); return CONTINU;}
	      else{return EXIT;}

	      break;
	    }
	    case id_HOME        : {setHomePosition(); return EXIT; break;}
	    case id_RESET       : {resetAll(); return EXIT; break;}
	    case id_RAZ         : {RAZ_All(); return EXIT; break;}
	    case id_NEW         : {return runWinding(FIRST_LUNCH, NEW	); break;}
	    case id_RESUME      : {return runWinding(FIRST_LUNCH, RESUME); break;}
	    case id_RESUME_SAVE : {return runWinding(FIRST_LUNCH, RESUME); break;}
	  }
	}
      // NO ___________________________________________________________________
      else if(buffercmp((char*)KEYWORD_NO, tmp_word, SIZE_KEYWORD_NO))
	{
	  switch (_tmp_id)
	  {
	    case id_SAVE	: {
	      if(_idValue == id_RESUME){ set_AB_SuspendMenu(); return CONTINU;}
	      else{ retry(); return CONTINU;}
	      break;
	    }
	    case id_HOME  : {return EXIT; break;}
	    case id_RESET : {return EXIT; break;}
	    case id_RAZ   : {return EXIT; break;}
	    case id_NEW   : {return EXIT; break;}
	  }
	}
      // SPEED _________________________________________________________________
      else if(buffercmp((char*)KEYWORD_SPEED, tmp_word, SIZE_KEYWORD_SPEED))
	{
	  if(id_SUSPEND){
	      adjustSpeed();
	      set_AB_SuspendMenu();
	      return CONTINU;
	  }
	  else return EXIT;
	}
      // EXIT __________________________________________________________________
      else if(buffercmp((char*)KEYWORD_EXIT, tmp_word, SIZE_KEYWORD_EXIT)){
	  return EXIT;}
      // NEXT __________________________________________________________________
      else if(buffercmp((char*)KEYWORD_NEXT, tmp_word, SIZE_KEYWORD_NEXT)){
	  return CONTINU;}
      // BACK __________________________________________________________________
      else if(buffercmp((char*)KEYWORD_BACK, tmp_word, SIZE_KEYWORD_BACK)){
	  if(_idValue == id_HOME)
	    {
	      set_AB_Home();
	      return CONTINU;
	    }
      }
      // Not found______________________________________________________________
      else return 0;
    }


  // Icons
  else
    {
      switch (_actionBar[_index])
      {
	// LEFT ________________________________________________________________
	case ICONLEFT[0] :
	{
	  if(_tmp_id == id_MOVE_CARRIAGE || _tmp_id == id_MOVE_COIL){
	      moving(C_CLOCK); return CONTINU;
	  }
	  break;
	}
	// RIGHT _______________________________________________________________
	case ICONRIGHT[0] :
	{
	  if(_tmp_id == id_MOVE_CARRIAGE || _tmp_id == id_MOVE_COIL){
	      moving(CLOCK); return CONTINU;
	  }
	  else{
	      update(); return EXIT;
	  }
	  break;
	}
	// RESUME ______________________________________________________________
	case ICONRESUME[0] :
	{
	  if(_tmp_id == id_SUSPEND){ return runWinding(RESUME, RESUME);}
	  break;
	}
	// STOP ________________________________________________________________
	case ICONSTOP[0] :
	{
	  return EXIT;
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
  _Display->engineSetValue(_label, _actionBar,_positionAB);
}


/******************************************************************************
 * brief   : Set action bar to move carriage
 ******************************************************************************/
void Setting::set_AB_MoveCar()
{
  _tmp_id = id_MOVE_CARRIAGE;
  setValues(MSG_MOVE, COUNTOF(INIT_MOV_CAR), UNIT_MM, ACTIONBAR_MOVE_BACK, SIZE_AB_MOVE_BACK,
	    LCD_LINES);
  displaying();
}


/******************************************************************************
 * brief   : If user click on "Back" after set carriage position.
 ******************************************************************************/
void Setting::set_AB_Home()
{
  setValues(MSG_HOMING, ACTIONBAR_SETHOME, SIZE_AB_SETHOME, LCD_LINES);
  displaying();
}


/******************************************************************************
 * brief   : Sub-menu setHome
 * details :
 ******************************************************************************/
void Setting::set_AB_SetHome()
{
  _tmp_id = id_HOME;

  setValues(MSG_CONFIRM, ACTIONBAR_CHOICE, SIZE_AB_CHOICE, LCD_LINES);
  displaying();
}


/******************************************************************************
 * brief   : Sub menu save
 * details : Print current value and ask save? if yes save in eeprom memory.
 ******************************************************************************/
void Setting::set_AB_Save()
{
  _tmp_id = id_SAVE;

  if(_idValue != id_RESUME)
    {
      update();
      setActionBar(0, 0, ACTIONBAR_CHOICE, SIZE_AB_CHOICE, LCD_LINES);
      _Display->engineSave(*p_floatingValue, _unit,_actionBar, _positionAB);
    }
  else
    {
      TotalSteps     = _Coil->getTotalStepsCounter();
      LayerSteps     = _Coil->getLayerStepsCounter();
      LayerCoilSteps = _Coil->getLayerCoilStepsCounter();
      setActionBar(0, 0, ACTIONBAR_CHOICE, SIZE_AB_CHOICE, LCD_LINES);
      _Display->engineSaveCurrent(_actionBar, _positionAB, Turns, _Coil->getCurrentTurns());
    }
}


/******************************************************************************
 * brief   : Set the homing position
 ******************************************************************************/
void Setting::setHomePosition()
{
  _homePosition = 0;
  _Display->loadBar();
}


/******************************************************************************
 * brief   : Save current value.
 * details : Store current value in EEprom memory.
 ******************************************************************************/
void Setting::saveCurrent()
{
  if(_idValue == id_RESUME) save(p_arrayValue, id_RESUME_SAVE);
  else save(p_arrayValue, _idValue);

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


/******************************************************************************
 * brief   : Prepare and lunch movement on coil or carriage
 * details : Update all values needed for moving coil or carriage.
 ******************************************************************************/
void Setting::moving(bool direction)
{
  update();

  // set speed.
  adjustSpeed();

  // Message displacement.
  _Display->engineMoving(*p_floatingValue, _unit, direction);

  // Start
  if(_idValue == id_MOVE_CARRIAGE || _tmp_id == id_MOVE_CARRIAGE)
    {
      _Coil->runOnlyCarriage(direction, *p_floatingValue);
    }
  else
    {
      _Coil->runOnlyCoil(direction, *p_floatingValue);
    }

  _Coil->disableMotors();
  displaying();
}


/******************************************************************************
 * brief   : Main winding function
 * details : Set all value for winding
 ******************************************************************************/
bool Setting::runWinding(bool isFirstLunch, bool isNewCoil)
{

  // Pass values
  setWinding(isFirstLunch);

  // Start winding with "runMultiLayer()"
  if( _Coil->runMultiLayer(isNewCoil) == false) // false if all is ok
    {
      _Coil->disableMotors(); // "runMultiLayer()" return true if winding is finished.
      return EXIT;
    }
  else
    {
      set_AB_SuspendMenu();
      return CONTINU;
    }
}


/******************************************************************************
 * brief   : Set value for lunch the winding.
 * details : Read eeprom if we lunch a saved session.
 ******************************************************************************/
void Setting::setWinding(bool isFirstLunch)
{
  // Adjust speed sub-menu for the first use
  if(isFirstLunch) adjustSpeed();

  _Coil->setWinding(CoilLength, WireSize, Turns, WinSense, CarSense);

  // Resume a saved session
  if(_idValue == id_RESUME_SAVE)
    {
      read(0, id_RESUME_SAVE); // Read the value of all steps in eeprom
      _Coil->setSteps(TotalSteps, LayerSteps, LayerCoilSteps);
      _idValue = id_RESUME;
    }
}


/******************************************************************************
 * brief   : Adjust speed winding
 * details : Set the speed of current winding.
 ******************************************************************************/
void Setting::adjustSpeed()
{
  bool refresh = true;
  int16_t oldSpeed = 0;

  _Display->engineAjustSpeed( !refresh, _speedPercent);

  while( _Encoder->getButton() != ClickEncoder::Clicked) // Click for exit
    {
      // Value increase when you turn encoder
      _speedPercent += _Encoder->getValue();
      // Clamp to 1% at 100%
      clampValue(&_speedPercent, 1, 100);
      // Refresh LCD only if value change
      if(_speedPercent != oldSpeed){
	  _Display->engineAjustSpeed(refresh, _speedPercent);
      }

      oldSpeed = _speedPercent;
    }

  oldSpeed = map(_speedPercent, 0, 100, MinSpeed, MaxSpeed);

  _Coil->setSpeed(AccDelay, MaxSpeed, MinSpeed, oldSpeed);
}


/******************************************************************************
 * brief   : Menu suspend
 * details : Enter in menu suspend if user click on encoder during winding.
 ******************************************************************************/
void Setting::set_AB_SuspendMenu()
{
  _idValue = id_RESUME; // For recovery the current winding
  _tmp_id = id_SUSPEND; // For execute menuSuspend();

  setActionBar(0, 0, ACTIONBAR_SUSPEND, SIZE_AB_SUSPEND, 0); // Displaying on the top
  _Display->engineSuspend(_actionBar, _positionAB, Turns, _Coil->getCurrentTurns());
}
