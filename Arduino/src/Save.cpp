/*
 * Save.cpp
 *
 *  Created on: 5 déc. 2016
 *      Author: jo
 */

#include "Save.h"

Memory::Memory() :
_addr_WireSize(0),
_addr_CoilLength(0),
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
  _addr_WireSize 	= EEPROM.getAddress(sizeof(char)*BUFFSIZE_WIRE);
  _addr_CoilLength 	= EEPROM.getAddress(sizeof(char)*BUFFSIZE_COIL);
  _addr_Turns 		= EEPROM.getAddress(sizeof(char)*BUFFSIZE_TURNS);
  _addr_MaxSpeed 	= EEPROM.getAddress(sizeof(char)*BUFFSIZE_MAX_SPEED);
  _addr_MinSpeed 	= EEPROM.getAddress(sizeof(char)*BUFFSIZE_MIN_SPEED);
  _addr_AccDelay 	= EEPROM.getAddress(sizeof(char)*BUFFSIZE_ACC_DELAY);

  _addr_DefaultSettings = EEPROM.getAddress(sizeof(char)*BUFFSIZE_DEFAULT);

  updateValue();

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

void Memory::save(char buffer[], const uint8_t id)
{
  switch (id)
  {
    case id_WIRESIZE :
      {
	EEPROM.updateBlock<char>(_addr_WireSize, buffer, BUFFSIZE_WIRE);
	break;
      }
    case id_COILLENGTH :
      {
	EEPROM.updateBlock<char>(_addr_CoilLength, buffer, BUFFSIZE_COIL);
	break;
      }
    case id_TURNS :
      {
	EEPROM.updateBlock<char>(_addr_Turns, buffer, BUFFSIZE_TURNS);
	break;
      }
    case id_MAX_SPEED :
      {
	EEPROM.updateBlock<char>(_addr_MaxSpeed, buffer, BUFFSIZE_MAX_SPEED);
	break;
      }
    case id_MIN_SPEED :
      {
	EEPROM.updateBlock<char>(_addr_MinSpeed, buffer, BUFFSIZE_MIN_SPEED);
	break;
      }
    case id_ACC_DELAY :
      {
	EEPROM.updateBlock<char>(_addr_AccDelay, buffer, BUFFSIZE_ACC_DELAY);
	break;
      }
  }
}

void Memory::read(char buffer[], const uint8_t id)
{
  switch (id)
  {
    case id_WIRESIZE :
      {
	EEPROM.readBlock<char>(_addr_WireSize, buffer, BUFFSIZE_WIRE);
	break;
      }
    case id_COILLENGTH :
      {
	EEPROM.readBlock<char>(_addr_CoilLength, buffer, BUFFSIZE_COIL);
	break;
      }
    case id_TURNS :
      {
	EEPROM.readBlock<char>(_addr_Turns, buffer, BUFFSIZE_TURNS);
	break;
      }
    case id_MAX_SPEED :
      {
	EEPROM.readBlock<char>(_addr_MaxSpeed, buffer, BUFFSIZE_MAX_SPEED);
	break;
      }
    case id_MIN_SPEED :
      {
	EEPROM.readBlock<char>(_addr_MinSpeed, buffer, BUFFSIZE_MIN_SPEED);
	break;
      }
    case id_ACC_DELAY :
      {
	EEPROM.readBlock<char>(_addr_AccDelay, buffer, BUFFSIZE_ACC_DELAY);
	break;
      }
  }
}

void Memory::updateValue()
{
  convertDataToFloat(_addr_WireSize, BUFFSIZE_WIRE, &WireSize);
  convertDataToFloat(_addr_CoilLength, BUFFSIZE_COIL, &CoilLength);
  convertDataToFloat(_addr_Turns, BUFFSIZE_TURNS, &Turns);
  convertDataToFloat(_addr_MaxSpeed, BUFFSIZE_MAX_SPEED, &MaxSpeed);
  convertDataToFloat(_addr_MinSpeed, BUFFSIZE_MIN_SPEED, &MinSpeed);
  convertDataToFloat(_addr_AccDelay, BUFFSIZE_ACC_DELAY, &AccDelay);
}

void Memory::reset()
{
  EEPROM.writeBlock<char>(_addr_WireSize, INIT_WIRE, BUFFSIZE_WIRE);
  EEPROM.writeBlock<char>(_addr_CoilLength, INIT_COIL, BUFFSIZE_COIL);
  EEPROM.writeBlock<char>(_addr_Turns, INIT_TURNS, BUFFSIZE_TURNS);
  EEPROM.writeBlock<char>(_addr_MaxSpeed, INIT_MAXSPEED, BUFFSIZE_MAX_SPEED);
  EEPROM.writeBlock<char>(_addr_MinSpeed, INIT_MINSPEED, BUFFSIZE_MIN_SPEED);
  EEPROM.writeBlock<char>(_addr_AccDelay, INIT_ACC_DELAY, BUFFSIZE_ACC_DELAY);

  EEPROM.writeBlock<char>(_addr_DefaultSettings, MSG_IS_SET, BUFFSIZE_DEFAULT);
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

/* PRIVATE -------------------------------------------------------------------*/

// Read eeprom, convert and store value in float
void Memory::convertDataToFloat(const uint8_t addr, const uint8_t bufferSize, float *usingData)
{
  char set[bufferSize];

  EEPROM.readBlock<char>(addr, set, bufferSize);

  *usingData = atof(set);
}

// Debug : Test reading and updating a string (char array) to EEPROM
void Memory::ReadCharArray() {
  char wire[BUFFSIZE_WIRE], coil[BUFFSIZE_COIL], turns[BUFFSIZE_TURNS],
  max[BUFFSIZE_MAX_SPEED], min[BUFFSIZE_MIN_SPEED], acc[BUFFSIZE_ACC_DELAY], set[BUFFSIZE_DEFAULT];

  EEPROM.readBlock<char>(_addr_WireSize, wire, BUFFSIZE_WIRE);
  EEPROM.readBlock<char>(_addr_CoilLength, coil, BUFFSIZE_COIL);
  EEPROM.readBlock<char>(_addr_Turns, turns, BUFFSIZE_TURNS);
  EEPROM.readBlock<char>(_addr_MaxSpeed, max, BUFFSIZE_MAX_SPEED);
  EEPROM.readBlock<char>(_addr_MinSpeed, min, BUFFSIZE_MIN_SPEED);
  EEPROM.readBlock<char>(_addr_AccDelay, acc, BUFFSIZE_ACC_DELAY);

  EEPROM.readBlock<char>(_addr_DefaultSettings, set, BUFFSIZE_DEFAULT);

  Serial.print("adress: ");
  Serial.print(_addr_WireSize);
  Serial.print("   buffer : ");
  Serial.println(wire);
  Serial.print("adress: ");
  Serial.print(_addr_CoilLength);
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



