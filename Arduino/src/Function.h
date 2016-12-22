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

// Macros to get simply the size of buffer.
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

#define EXIT false

void clampValue(int8_t *_val, uint8_t _min, uint8_t _max);
bool timer(unsigned long currentTime, unsigned long *oldTime, unsigned long delay);
uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);


#endif /* SRC_FUNCTION_H_ */
