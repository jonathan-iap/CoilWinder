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

void clampValue(int8_t *_val, uint8_t _min, uint8_t _max);
bool timer(unsigned long currentTime, unsigned long *oldTime, unsigned long delay);


#endif /* SRC_FUNCTION_H_ */
