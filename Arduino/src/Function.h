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
#include "Display.h"

// Macros to get simply the size of buffer with null character.
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

#define EXIT false
#define CONTINU true

void clampValue(int8_t *_val, uint8_t _min, uint8_t _max);
bool timer(unsigned long currentTime, unsigned long *oldTime, unsigned long delay);
uint16_t buffercmp(uint8_t* originArray, uint8_t* targetArray, uint16_t BufferLength);
void bufferCopy(uint8_t* originArray, uint8_t* targetArray, int8_t index, uint8_t wordSize);
uint8_t motionSense(int8_t index, int8_t lastIndex);
void ignoreChar(char array[], int8_t *index, uint8_t sense);
uint8_t wordDetect(char array[], int8_t *index, uint8_t sense);
bool isNumber(char array[], int8_t index);
bool isWord(char array[], int8_t index, uint8_t wordSize, char return_word[]);

#endif /* SRC_FUNCTION_H_ */
