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
  _addr_wireSize = EEPROM.getAddress(sizeof(char)*BUFFSIZE_WIRE);
  _addr_coilLength = EEPROM.getAddress(sizeof(char)*BUFFSIZE_COIL);
  _addr_Turns = EEPROM.getAddress(sizeof(char)*BUFFSIZE_TURNS);
  // todo
  _addr_MaxSpeed = EEPROM.getAddress(sizeof(char)*6);// not set properly !
  _addr_MinSpeed = EEPROM.getAddress(sizeof(char)*6);// not set properly !
  _addr_AccDelay = EEPROM.getAddress(sizeof(char)*6);// not set properly !

  _addr_DefaultSettings = EEPROM.getAddress(sizeof(char)*BUFFSIZE_DEFAULT);

  // If is the first use or if data are corrupted do reset.
  if(checkNoSettings()) reset();

}

void Memory::reset()
{

}

// Private
bool Memory::checkNoSettings()
{
  char set[BUFFSIZE_DEFAULT];
  EEPROM.readBlock<char>(_addr_DefaultSettings, set, BUFFSIZE_DEFAULT);

  if(Buffercmp((uint8_t*)MSG_IS_SET, (uint8_t*)set, BUFFSIZE_DEFAULT))
    {
      return true;
    }
  else return false;
}





