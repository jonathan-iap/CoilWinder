/*
 * Function.h
 *
 * General function.
 *
 *  Created on: 5 déc. 2016
 *      Author: jo
 */

#ifndef SRC_FUNCTION_H_
#define SRC_FUNCTION_H_

#include "Arduino.h"

#define id_WIRESIZE	1
#define id_COILLENGTH	2
#define id_TURNS	3
#define id_MAX_SPEED	4
#define id_MIN_SPEED	5
#define id_ACC_DELAY	6

/******************************************************************************
 * brief   : Macros to get simply the size of a buffer.
 * details : __BUFFER__ , buffer of which we wants to know the size
 * return  : size of buffer
 ******************************************************************************/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))


void clampValue(int8_t *_val, uint8_t _min, uint8_t _max);
bool timer(unsigned long currentTime, unsigned long *oldTime, unsigned long delay);
uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);


#endif /* SRC_FUNCTION_H_ */
