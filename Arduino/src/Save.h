/*
 * Save.h
 *
 *  Created on: 5 déc. 2016
 *      Author: jo
 */

#ifndef SRC_SAVE_H_
#define SRC_SAVE_H_

#include "Arduino.h"
#include "EEPROMex.h"
#include "Function.h"


// First EEprom address to start
#define MEN_BASE  100
// Value for initialization of menu
#define INIT_WIRE	"0.00"
#define INIT_COIL	"000.00"
#define INIT_TURNS	"00000"
#define INIT_MAXSPEED	"0000"
#define INIT_MINSPEED	"0000"
#define INIT_ACC_DELAY	"0000"
#define MSG_IS_SET   	"is set"
// Size for reserve memory
#define BUFFSIZE_WIRE    	(COUNTOF(INIT_WIRE))
#define BUFFSIZE_COIL    	(COUNTOF(INIT_COIL))
#define BUFFSIZE_TURNS  	(COUNTOF(INIT_TURNS))
#define BUFFSIZE_MAX_SPEED	(COUNTOF(INIT_MAXSPEED))
#define BUFFSIZE_MIN_SPEED	(COUNTOF(INIT_MINSPEED))
#define BUFFSIZE_ACC_DELAY	(COUNTOF(INIT_ACC_DELAY))
#define BUFFSIZE_DEFAULT 	(COUNTOF(MSG_IS_SET))


class Memory
{
public :

  Memory();
  ~Memory();

  void init();
  void save();
  void read();
  bool isSet();
  void reset();

private :
  uint8_t _addr_wireSize;
  uint8_t _addr_coilLength;
  uint8_t _addr_Turns;
  uint8_t _addr_MaxSpeed;
  uint8_t _addr_MinSpeed;
  uint8_t _addr_AccDelay;
  uint8_t _addr_DefaultSettings;

  char _value[];

  void ReadCharArray();

};


#endif /* SRC_SAVE_H_ */
