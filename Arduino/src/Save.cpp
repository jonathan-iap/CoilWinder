/*
 * Save.cpp
 *
 *  Created on: 5 déc. 2016
 *      Author: jo
 */

#include "Save.h"

Memory::Memory()
: WireSize(0),
  CoilLength(0),
  Turns(0),
  MaxSpeed(0),
  MinSpeed(0),
  AccDelay(0)
{
  // start reading from position memBase (address 0) of the EEPROM. Set maximumSize to EEPROMSizeUno
  // Writes before membase or beyond EEPROMSizeUno will only give errors when _EEPROMEX_DEBUG is set
  EEPROM.setMemPool(MEN_BASE, EEPROMSizeUno);

  _addr_WireSize=EEPROM.getAddress(sizeof(char)*BUFFSIZE_WIRE);
  _addr_CoilLength=EEPROM.getAddress(sizeof(char)*BUFFSIZE_COIL);
  _addr_Turns=EEPROM.getAddress(sizeof(char)*BUFFSIZE_TURNS);
  _addr_MaxSpeed=EEPROM.getAddress(sizeof(char)*BUFFSIZE_MAX_SPEED);
  _addr_MinSpeed=EEPROM.getAddress(sizeof(char)*BUFFSIZE_MIN_SPEED);
  _addr_AccDelay=EEPROM.getAddress(sizeof(char)*BUFFSIZE_ACC_DELAY);
  _addr_DefaultSettings=EEPROM.getAddress(sizeof(char)*BUFFSIZE_DEFAULT);

  // If is the first use or if data are corrupted do reset.
  if( !isSet() ) reset();

  // Read and set all data saved in eeprom memory.
  readAll();
}
Memory::~Memory(){}

void Memory::save(char buffer[], const uint8_t id)
{
  //ReadAddresses();
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

void Memory::readAll()
{
  // Read the eeprom memory and set buffers with values saved
  read(_buff_WireSize,id_WIRESIZE);
  read(_buff_CoilLength, id_COILLENGTH);
  read(_buff_Turns, id_TURNS);
  read(_buff_MaxSpeed,id_MAX_SPEED);
  read(_buff_MinSpeed, id_MIN_SPEED);
  read(_buff_AccDelay, id_ACC_DELAY);
  // Convert value within array in a double value.
  WireSize 	= atof(_buff_WireSize);
  CoilLength 	= atof(_buff_CoilLength);
  Turns 	= atof(_buff_Turns);
  MaxSpeed 	= atof(_buff_MaxSpeed);
  MinSpeed 	= atof(_buff_MinSpeed);
  AccDelay 	= atof(_buff_AccDelay);
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
// convert and format float to data
void Memory::writeFloatToData(double data, char buffer[], const uint8_t bufferSize)
{
  dtostrf(data, (bufferSize-1), 2, buffer);

  for(uint8_t i=bufferSize; i>0; i--)
    {
      buffer[i] == ' ' ? buffer[i] = '0' : false;
    }
}

void Memory::ReadAddresses()
{
  Serial.print("adress wire: "); Serial.println(_addr_WireSize);
  Serial.print("adress coil: "); Serial.println(_addr_CoilLength);
  Serial.print("adress turns: "); Serial.println(_addr_Turns);
  Serial.print("adress max: "); Serial.println(_addr_MaxSpeed);
  Serial.print("adress min: "); Serial.println(_addr_MinSpeed);
  Serial.print("adress acc: "); Serial.println(_addr_AccDelay);
  Serial.print("adress default: "); Serial.println(_addr_DefaultSettings);
}

void Memory::ReadFloatValue()
{
  Serial.print("float wire: "); Serial.println(WireSize);
  Serial.print("float coil: "); Serial.println(CoilLength);
  Serial.print("float turns: "); Serial.println(Turns);
  Serial.print("float max: "); Serial.println(MaxSpeed);
  Serial.print("float min: "); Serial.println(MinSpeed);
  Serial.print("float acc: "); Serial.println(AccDelay);
}

void Memory::ReadArrayValue()
{
  char _buff_DefaultSettings[BUFFSIZE_DEFAULT] = {0};

  Serial.print("array wire: "); Serial.println(_buff_WireSize);
  Serial.print("array coil: "); Serial.println(_buff_CoilLength);
  Serial.print("array turns: "); Serial.println(_buff_Turns);
  Serial.print("array max: "); Serial.println(_buff_MaxSpeed);
  Serial.print("array min: "); Serial.println(_buff_MinSpeed);
  Serial.print("array acc: "); Serial.println(_buff_AccDelay);
  EEPROM.readBlock<char>(_addr_DefaultSettings, _buff_DefaultSettings, BUFFSIZE_DEFAULT);
  Serial.print("array default: "); Serial.println(_buff_DefaultSettings);
}
