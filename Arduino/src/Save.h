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

#define id_WIRESIZE	1
#define id_COILLENGTH	2
#define id_TURNS	3
#define id_MAX_SPEED	4
#define id_MIN_SPEED	5
#define id_ACC_DELAY	6

class Memory
{
public :

  Memory();
  ~Memory();

  void init();
  void save(char buffer[], const uint8_t id);
  void read(char buffer[], const uint8_t id);
  void reset();
  bool isSet();

protected :
  static double WireSize;
  static double CoilLength;
  static double Turns;
  static double MaxSpeed;
  static double MinSpeed;
  static double AccDelay;

  // Init buffers
  static char _buff_WireSize[BUFFSIZE_WIRE];
  static char _buff_CoilLength[BUFFSIZE_COIL];
  static char _buff_Turns[BUFFSIZE_TURNS];
  static char _buff_MaxSpeed[BUFFSIZE_MAX_SPEED];
  static char _buff_MinSpeed[BUFFSIZE_MIN_SPEED];
  static char _buff_AccDelay[BUFFSIZE_ACC_DELAY];

private :
  const uint8_t _addr_WireSize;
  const uint8_t _addr_CoilLength;
  const uint8_t _addr_Turns;
  const uint8_t _addr_MaxSpeed;
  const uint8_t _addr_MinSpeed;
  const uint8_t _addr_AccDelay;
  const uint8_t _addr_DefaultSettings;

  void writeFloatToData(double Data, char buffer[], const uint8_t bufferSize);

  // for debug
  void ReadAddresses();
  void ReadFloatValue();
  void ReadArrayValue();
};


#endif /* SRC_SAVE_H_ */
