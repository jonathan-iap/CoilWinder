/*
 * Display.h
 *
 *  Created on: 5 nov. 2016
 *      Author: jo
 */

#ifndef SRC_DISPLAY_H_
#define SRC_DISPLAY_H_

#include "Arduino.h"

#define IconLeft 	1
#define IconRight	2
#define IconBack	3
#define IconEnter	4

class Display
{
  // Public variables -----------------------------------------------------------
public :
  uint8_t left[8] = {
      0b00010,
      0b00110,
      0b01110,
      0b11110,
      0b01110,
      0b00110,
      0b00010,
      0b00000
  };

  uint8_t right[8] = {
      0b01000,
      0b01100,
      0b01110,
      0b01111,
      0b01110,
      0b01100,
      0b01000,
      0b00000
  };

  uint8_t back[8] = {
      0b00000,
      0b00100,
      0b01100,
      0b11111,
      0b01101,
      0b00101,
      0b00001,
      0b00000
  };

  uint8_t enter[8] = {
      0b00000,
      0b10000,
      0b10100,
      0b10110,
      0b11111,
      0b00110,
      0b00100,
      0b00000
  };

  // Public functions -----------------------------------------------------------
public:
  Display();
  const void begin();


private:

};

#endif /* SRC_DISPLAY_H_ */
