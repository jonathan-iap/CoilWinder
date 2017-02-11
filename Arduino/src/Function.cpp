/*
 * Function.cpp
 *
 *  Created on: 5 déc. 2016
 *      Author: jo
 */

#include "Function.h"

/******************************************************************************
 * brief   : Clamp value on a positive interval
 * details : _val, value that we want clamp is change by pointer.
 * _min, minimal value for _val
 * _max, maximal value for _val
 * return  : nothing
 ******************************************************************************/
void clampValue(int8_t *_val, uint8_t _min, uint8_t _max)
{
  if(*_val > _max){ *_val = _max;}
  else if (*_val < _min){ *_val = _min;}
  else;
}

/******************************************************************************
 * brief   : Run action without delay.
 * details : timer() function need to be used in a while() loop and
 * often refresh. When time reach timer() return true.
 * currentTime, time to function "micros()". Refresh as often as possible !
 * oldTime, pointer to the variable who contain the old value of _currentTime.
 * this value is modified when time reach.
 * delay, delay before you do instructions.
 * return  : True when time reach.
 ******************************************************************************/
bool timer(unsigned long currentTime, unsigned long *oldTime, unsigned long delay)
{
  if (currentTime - *oldTime >= delay)
    {
      *oldTime = currentTime;
      return true;
    }
  return false;
}


/******************************************************************************
 * brief   : Compare the content of two buffer.
 * details : originArray, buffer one is the reference
 * targetArray, buffer to compare
 * BufferLength, length of buffer one.
 * return  : 1 if the two buffers are equals.
 ******************************************************************************/
uint16_t buffercmp(uint8_t* originArray, uint8_t* targetArray, uint16_t BufferLength)
{
  while(BufferLength--)
    {
      Serial.print("cmp A1 et A2 : "); Serial.print(*originArray);
      Serial.print("/"); Serial.println(*targetArray);

      if((*originArray) != *targetArray)
	{
	  return false;
	}
      originArray++;
      targetArray++;
    }

  return true;
}


/******************************************************************************
 * brief   : Copy two buffer.
 * details : Allows you to copy only one part of buffer origin
 * into a target array.
 * return  : Buffer is directly copy into an other.
 ******************************************************************************/
void bufferCopy(uint8_t* originArray, uint8_t* targetArray, int8_t index, uint8_t wordSize)
{
  uint8_t count = 0;

  for(uint8_t i=index; i<(index+wordSize); i++)
    {
      targetArray[count] = originArray[i];
      count++;
    }
}


/******************************************************************************
 * brief   : Detect if the cursor is moving.
 * details : If the index change we determine if the cursor move left or right
 * or
 * return  : New position for cursor.
 ******************************************************************************/
uint8_t motionSense(int8_t index, int8_t lastIndex)
{
  // Move left to right
  if(index > lastIndex)
    {
      return CURSOR_MOVE_RIGHT;
    }
  // Move right to left
  else if (index < lastIndex)
    {
      return  CURSOR_MOVE_LEFT;
    }
  // No movement
  else return false;
}


/******************************************************************************
 * brief   : Ignore unwanted character.
 * details : If cursor is on unwanted character We ignore it by jumping
 * from an index.
 * return  : New position for cursor by the pointer "*index".
 ******************************************************************************/
void ignoreChar(char array[], int8_t *index, uint8_t sense)
{
  // Ignored character.
  while(array[*index] == '.' || array[*index] == '/' || array[*index] == ' ')
    {
      sense == CURSOR_MOVE_RIGHT ? *index += 1 : *index -= 1;
    }
}


/******************************************************************************
 * brief   : Detect if cursor is on word
 * details : If the cursor is at the beginning of a word, we browse the word
 * until the last character.
 * return  : The size of word and 0 if is just a single character.
 ******************************************************************************/
uint8_t wordDetect(char array[], int8_t *index, uint8_t sense)
{
  bool isCharacter = false;
  uint8_t wordSize = 0;

  // Characters counting
  while((array[*index] > 64 && array[*index] < 91)	 // Upper case
      || (array[*index] > 96 && array[*index] < 123))  // Lower case
    {
      sense == CURSOR_MOVE_RIGHT ? *index += 1 : *index -=1;

      wordSize ++;
      isCharacter = true;
    }

  if(isCharacter)
    {
      // Set index at the beginning of the word
      sense == CURSOR_MOVE_RIGHT ? *index -=wordSize : *index +=1;
    }

  if(wordSize > 1) return wordSize;
  else return false; // Single character.
}


/******************************************************************************
 * brief   : Checks if it's a number
 * details : Checks if character in the array at the index is a number
 * return  : true if yes.
 ******************************************************************************/
bool isNumber(char array[], int8_t index)
{
  if(array[index]<58 && array[index]>47) return true;
  else return false;
}


/******************************************************************************
 * brief   : Checks if it's a word
 * details : Checks if the current cursor is on word
 * return  : true if yes and fill "tmp_word[]" with "array"
 ******************************************************************************/
bool isWord(char array[], int8_t index, uint8_t wordSize, char return_word[])
{
  if(wordSize>1)
    {
      bufferCopy((uint8_t*)array, (uint8_t*)return_word, index, wordSize);
      return true;
    }
  else return false;
}
