/*
 * Save.cpp
 *
 *  Created on: 5 déc. 2016
 *      Author: jo
 */

#include "Save.h"

Memory::Memory() :
_addr_wireSize(0),
_addr_coilLength(0),
_addr_Turns(0),
_addr_MaxSpeed(0),
_addr_MinSpeed(0),
_addr_AccDelay(0),
_addr_DefaultSettings(0)
{};
Memory::~Memory(){};

void Memory::init()
{
  // start reading from position memBase (address 0) of the EEPROM. Set maximumSize to EEPROMSizeUno
  // Writes before membase or beyond EEPROMSizeUno will only give errors when _EEPROMEX_DEBUG is set
  EEPROM.setMemPool(MEN_BASE, EEPROMSizeUno);
  _addr_wireSize 	= EEPROM.getAddress(sizeof(char)*BUFFSIZE_WIRE);
  _addr_coilLength 	= EEPROM.getAddress(sizeof(char)*BUFFSIZE_COIL);
  _addr_Turns 		= EEPROM.getAddress(sizeof(char)*BUFFSIZE_TURNS);
  _addr_MaxSpeed 	= EEPROM.getAddress(sizeof(char)*BUFFSIZE_MAX_SPEED);
  _addr_MinSpeed 	= EEPROM.getAddress(sizeof(char)*BUFFSIZE_MIN_SPEED);
  _addr_AccDelay 	= EEPROM.getAddress(sizeof(char)*BUFFSIZE_ACC_DELAY);

  _addr_DefaultSettings = EEPROM.getAddress(sizeof(char)*BUFFSIZE_DEFAULT);

#ifdef DEBUG
  Serial.println("---------------------------------");
  Serial.println("Reading a char array, before :");
  Serial.println("---------------------------------");
  ReadCharArray();
#endif

  // If is the first use or if data are corrupted do reset.
  if( !isSet() ) reset();

#ifdef DEBUG
  Serial.println("---------------------------------");
  Serial.println("Reading a char array, after : ");
  Serial.println("---------------------------------");
  ReadCharArray();
#endif
}

bool Memory::isSet()
{
  char set[BUFFSIZE_DEFAULT];
  EEPROM.readBlock<char>(_addr_DefaultSettings, set, BUFFSIZE_DEFAULT);

  if(Buffercmp((uint8_t*)MSG_IS_SET, (uint8_t*)set, BUFFSIZE_DEFAULT))
    {
      return false;
    }
  else return true;
}

void Memory::reset()
{
  EEPROM.writeBlock<char>(_addr_wireSize, INIT_WIRE, BUFFSIZE_WIRE);
  EEPROM.writeBlock<char>(_addr_coilLength, INIT_COIL, BUFFSIZE_COIL);
  EEPROM.writeBlock<char>(_addr_Turns, INIT_TURNS, BUFFSIZE_TURNS);
  EEPROM.writeBlock<char>(_addr_MaxSpeed, INIT_MAXSPEED, BUFFSIZE_MAX_SPEED);
  EEPROM.writeBlock<char>(_addr_MinSpeed, INIT_MINSPEED, BUFFSIZE_MIN_SPEED);
  EEPROM.writeBlock<char>(_addr_AccDelay, INIT_ACC_DELAY, BUFFSIZE_ACC_DELAY);

  EEPROM.writeBlock<char>(_addr_DefaultSettings, MSG_IS_SET, BUFFSIZE_DEFAULT);
}


/* PRIVATE -------------------------------------------------------------------*/

// Debug : Test reading and updating a string (char array) to EEPROM
void Memory::ReadCharArray() {
  char wire[BUFFSIZE_WIRE], coil[BUFFSIZE_COIL], turns[BUFFSIZE_TURNS],
  max[BUFFSIZE_MAX_SPEED], min[BUFFSIZE_MIN_SPEED], acc[BUFFSIZE_ACC_DELAY], set[BUFFSIZE_DEFAULT];

  EEPROM.readBlock<char>(_addr_wireSize, wire, BUFFSIZE_WIRE);
  EEPROM.readBlock<char>(_addr_coilLength, coil, BUFFSIZE_COIL);
  EEPROM.readBlock<char>(_addr_Turns, turns, BUFFSIZE_TURNS);
  EEPROM.readBlock<char>(_addr_MaxSpeed, max, BUFFSIZE_MAX_SPEED);
  EEPROM.readBlock<char>(_addr_MinSpeed, min, BUFFSIZE_MIN_SPEED);
  EEPROM.readBlock<char>(_addr_AccDelay, acc, BUFFSIZE_ACC_DELAY);

  EEPROM.readBlock<char>(_addr_DefaultSettings, set, BUFFSIZE_DEFAULT);

  Serial.print("adress: ");
  Serial.print(_addr_wireSize);
  Serial.print("   buffer : ");
  Serial.println(wire);
  Serial.print("adress: ");
  Serial.print(_addr_coilLength);
  Serial.print("   buffer : ");
  Serial.println(coil);
  Serial.print("adress: ");
  Serial.print(_addr_Turns);
  Serial.print("   buffer : ");
  Serial.println(turns);
  Serial.print("adress: ");
  Serial.print(_addr_MaxSpeed);
  Serial.print("   buffer : ");
  Serial.println(max);
  Serial.print("adress: ");
  Serial.print(_addr_MaxSpeed);
  Serial.print("   buffer : ");
  Serial.println(min);
  Serial.print("adress: ");
  Serial.print(_addr_AccDelay);
  Serial.print("   buffer : ");
  Serial.println(acc);
  Serial.print("adress: ");
  Serial.print(_addr_DefaultSettings);
  Serial.print("   buffer : ");
  Serial.println(set);

}



