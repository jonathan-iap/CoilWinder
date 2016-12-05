/*
 * Save.cpp
 *
 *  Created on: 5 déc. 2016
 *      Author: jo
 */

#include "Save.h"
#include "EEPROMex.h"

struct memAdress_t
{
  uint8_t addr_wireSize;
  uint8_t addr_coilLength;
  uint8_t addr_Turns;
  uint8_t addr_MaxSpeed;
  uint8_t addr_MinSpeed;
  uint8_t addr_AccDelay;
  uint8_t addr_DefaultSettings;
};

void memoryInit()
{
  // Structure EEprom memory
  memAdress_t memAddress;
  // start reading from position memBase (address 0) of the EEPROM. Set maximumSize to EEPROMSizeUno
  // Writes before membase or beyond EEPROMSizeUno will only give errors when _EEPROMEX_DEBUG is set
  EEPROM.setMemPool(MEN_BASE, EEPROMSizeUno);
  memAddress.addr_wireSize = EEPROM.getAddress(sizeof(char)*NUM_WIRE);
  memAddress.addr_coilLength = EEPROM.getAddress(sizeof(char)*NUM_COIL);
  memAddress.addr_Turns = EEPROM.getAddress(sizeof(char)*NUM_TURNS);
  memAddress.addr_MaxSpeed = EEPROM.getAddress(sizeof(char)*6);
  memAddress.addr_MinSpeed = EEPROM.getAddress(sizeof(char)*6);
  memAddress.addr_DefaultSettings = EEPROM.getAddress(sizeof(char)*6);

}
