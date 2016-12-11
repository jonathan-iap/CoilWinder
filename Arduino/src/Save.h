/*
 * Save.h
 *
 *  Created on: 5 déc. 2016
 *      Author: jo
 */

#ifndef SRC_SAVE_H_
#define SRC_SAVE_H_

#include "Arduino.h"
#include "Configuration.h"
#include "EEPROMex.h"
#include "Function.h"

// First EEprom address to start
#define MEN_BASE  50
// String to check if EEprom memory is initialize
#define MSG_IS_SET "is set"
// Size to store memory
#define BUFFSIZE_WIRE    	(COUNTOF(INIT_WIRE))
#define BUFFSIZE_COIL    	(COUNTOF(INIT_COIL))
#define BUFFSIZE_TURNS  	(COUNTOF(INIT_TURNS))
#define BUFFSIZE_MAX_SPEED	(COUNTOF(INIT_MAXSPEED))
#define BUFFSIZE_MIN_SPEED	(COUNTOF(INIT_MINSPEED))
#define BUFFSIZE_ACC_DELAY	(COUNTOF(INIT_ACC_DELAY))
#define BUFFSIZE_DEFAULT 	(COUNTOF(MSG_IS_SET))

extern float WireSize;
extern float CoilLength;
extern float Turns;
extern float MaxSpeed;
extern float MinSpeed;
extern float AccDelay;

class Memory
{
public :

  Memory();
  ~Memory();

  void init();
  void save(char buffer[], const uint8_t id);
  void read(char buffer[], const uint8_t id);
  void updateValue();
  void reset();
  bool isSet();



private :
  uint8_t _addr_WireSize;
  uint8_t _addr_CoilLength;
  uint8_t _addr_Turns;
  uint8_t _addr_MaxSpeed;
  uint8_t _addr_MinSpeed;
  uint8_t _addr_AccDelay;
  uint8_t _addr_DefaultSettings;

  char _value[];

  void convertDataToFloat(const uint8_t addr, const uint8_t bufferSize, float *usingData);

  // for debug
  void ReadCharArray();

};


#endif /* SRC_SAVE_H_ */
