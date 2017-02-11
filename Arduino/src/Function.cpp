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
