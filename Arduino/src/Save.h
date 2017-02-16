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

// Start EEprom address
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

/* ID _________________________________________________________________*/
// Values
#define id_WIRESIZE	1
#define id_COILLENGTH	2
#define id_TURNS	3
#define id_MAX_SPEED	4
#define id_MIN_SPEED	5
#define id_ACC_DELAY	6
// Actions
#define id_SAVE			20
#define id_RESET		21
#define id_RAZ			22
#define id_MOVE_CARRIAGE	23
#define id_MOVE_COIL		24
#define id_NEW			25
#define id_RESUME		26
#define id_RESUME_SAVE		27
#define id_SUSPEND		28

#define id_TEST		50

class Memory
{
public :

  Memory();
  ~Memory();

  void save(char buffer[], const uint8_t id);
  void read(char buffer[], const uint8_t id);
  void readAll();
  void reset();
  bool isSet();

  // for debug
  void ReadAddresses();
  void ReadFloatValue();
  void ReadArrayValue();

protected :
  float WireSize;
  float CoilLength;
  float Turns;
  float MaxSpeed;
  float MinSpeed;
  float AccDelay;
  uint32_t TotalSteps;
  uint32_t LayerSteps;

  char _buff_WireSize[BUFFSIZE_WIRE+1];
  char _buff_CoilLength[BUFFSIZE_COIL+1];
  char _buff_Turns[BUFFSIZE_TURNS+1];
  char _buff_MaxSpeed[BUFFSIZE_MAX_SPEED+1];
  char _buff_MinSpeed[BUFFSIZE_MIN_SPEED+1];
  char _buff_AccDelay[BUFFSIZE_ACC_DELAY+1];

private :
  uint8_t _addr_WireSize;
  uint8_t _addr_CoilLength;
  uint8_t _addr_Turns;
  uint8_t _addr_MaxSpeed;
  uint8_t _addr_MinSpeed;
  uint8_t _addr_AccDelay;
  uint8_t _addr_DefaultSettings;
  uint8_t _addr_TotalSteps;
  uint8_t _addr_LayerSteps;

  void writeFloatToData(float Data, char buffer[], const uint8_t bufferSize);
};

#endif /* SRC_SAVE_H_ */
