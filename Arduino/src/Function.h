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
bool timer(uint32_t currentTime, uint32_t *oldTime, uint16_t delay);
bool buffercmp(char originArray[], char targetArray[], uint16_t BufferLength);
void bufferCopy(char originArray[], char targetArray[], int8_t index, uint8_t wordSize);
uint8_t motionSense(int8_t index, int8_t lastIndex);
void ignoreChar(char array[], int8_t *index, uint8_t sense);
uint8_t wordDetect(char array[], int8_t *index, uint8_t sense);
bool isNumber(char array[], int8_t index);
bool isWord(char array[], int8_t index, uint8_t wordSize, char return_word[]);

#endif /* SRC_FUNCTION_H_ */
