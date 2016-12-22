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
 * details : pBuffer1, buffer one is the reference
 * pBuffer2, buffer to compare
 * BufferLength, length of buffer one.
 * return  : 0 if the two buffers are equals.
 ******************************************************************************/
uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while(BufferLength--)
    {
      if((*pBuffer1) != *pBuffer2)
	{
	  return BufferLength;
	}
      pBuffer1++;
      pBuffer2++;
    }

  return 0;
}
