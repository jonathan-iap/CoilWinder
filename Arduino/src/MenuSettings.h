/*
 * MenuSettings.h
 *
 *  Created on: 2 déc. 2016
 *      Author: jo
 */

#ifndef SRC_MENUSETTINGS_H_
#define SRC_MENUSETTINGS_H_

#include "Arduino.h"
#include "Function.h"

class Setting
{
public :

  Setting();
  ~Setting();

  float setValue(char value[], int arraySize, char valueName[]);

private:
};

#endif /* SRC_MENUSETTINGS_H_ */
