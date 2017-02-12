/*
 * MenuStructure.h
 *
 *  Created on: 21 déc. 2016
 *      Author: jo
 */

#ifndef SRC_MENUSTRUCTURE_H_
#define SRC_MENUSTRUCTURE_H_

#include "MenuSettings.h"
#include "Menu.h"
#include "Winding.h"

#define state_NONE 	0
#define state_DEFAULT 	1
#define state_MOVE	2
#define state_EDIT	3
#define state_BACK	4

#define BACK	"Back" ICONLEFT

extern ClickEncoder Encoder;
extern Menu::Engine *engine;
extern Display display;

extern uint8_t systemState;

Coil CoilWinding(&Encoder, &display);
Setting setting(&Encoder, &display, &CoilWinding);

// Item rendering --------------------------------------------------------------
void renderMenuItem(const Menu::Item_t *mi, uint8_t pos)
{
  bool current = false;
  // Print icon before current item else blank.
  engine->currentItem == mi ? current = true : current = false;
  display.renderIconOn(pos, current);

  // Print label item
  display.renderItem(engine->getLabel(mi));

  // mark items that have children
  engine->getChild(mi) != &Menu::NullItem ? display.renderIconChild() : display.blank(LCD_CHARS);
}


// CallBacks -------------------------------------------------------------------
bool menuDummy(const Menu::Action_t a)
{
  // Do nothing;
  return true;
}

bool menuBack(const Menu::Action_t a)
{
  if (a == Menu::actionDisplay)
    {
      systemState = state_BACK;
    }
  return true;
}

bool editWire(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.setId(id_WIRESIZE);
    }
  return true;
}

bool editLength(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.setId(id_COILLENGTH);
    }
  return true;
}

bool editTurns(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.setId(id_TURNS);
    }
  return true;
}

bool runWinding(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.runWinding(0,0);
    }
  return true;
}

bool runResume(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.runWinding(1,0);
    }
  return true;
}

bool runSaved(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.runWinding(0,1);
    }
  return true;
}

bool editMaxSpeed(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.setId(id_MAX_SPEED);
    }
  return true;
}

bool editMinSpeed(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.setId(id_MIN_SPEED);
    }
  return true;
}

bool editAccTime(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.setId(id_ACC_DELAY);
    }
  return true;
}

bool menuMovCarriage(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.moveCarriage();
    }
  return true;
}

bool menuMovCoil(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.moveCoil();
    }
  return true;
}

bool menuReset(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.resetAction(false);
    }
  return true;
}

bool menuRAZ(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.resetAction(true);
    }
  return true;
}

bool reworkTest(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.editionMenu(id_WIRESIZE);
    }
  return true;
}

// Framework for menu ---------------------------------------------------------

// Name, Label, Next, Previous, Parent, Child, Callback
// Menu 0
MenuItem(miExit, "", Menu::NullItem, Menu::NullItem, Menu::NullItem, miWinding, menuDummy);
// Menu 1 -> 3
//MenuItem(miWinding, "Winding", miMoves, Menu::NullItem, miExit, miWireSize, menuDummy);
MenuItem(miMoves, "Moves", miSettings, miWinding, miExit, miMovCarriage, menuDummy);
MenuItem(miSettings, "Settings", Menu::NullItem, miMoves, miExit, miMaxSpeed, menuDummy);

// Sub-menu 1.1 -> 1.6
MenuItem(miWireSize, "1.Wire size", miCoilLength, Menu::NullItem, miWinding, Menu::NullItem, editWire);
MenuItem(miCoilLength, "2.Coil length", miTurns, miWireSize, miWinding, Menu::NullItem, editLength);
MenuItem(miTurns, "3.Turns", miStart, miCoilLength, miWinding, Menu::NullItem, editTurns);
MenuItem(miStart, "4.Start new", miResume, miTurns, miWinding, Menu::NullItem, runWinding);
MenuItem(miResume, "Resume current", miSaved, miStart, miWinding, Menu::NullItem, runResume);
MenuItem(miSaved, "Resume saved", miBack1, miResume, miWinding, Menu::NullItem, runSaved);
MenuItem(miBack1, BACK, Menu::NullItem, miStart, miWinding, Menu::NullItem, menuBack);
// Sub-menu 2.1 -> 2.?
MenuItem(miMovCarriage, "Move carriage", miMovCoil, Menu::NullItem, miMoves, Menu::NullItem, menuMovCarriage);
MenuItem(miMovCoil, "Move coil", miBack2, miMovCarriage, miMoves, Menu::NullItem, menuMovCoil);
MenuItem(miBack2, BACK, Menu::NullItem, miMovCoil, miMoves, Menu::NullItem, menuBack);
// Sub-menu 3.1 -> 3.?
MenuItem(miMaxSpeed, "Max speed", miMinSpeed, Menu::NullItem, miSettings, Menu::NullItem, editMaxSpeed);
MenuItem(miMinSpeed, "Min speed", miAccTime, miMaxSpeed, miSettings, Menu::NullItem, editMinSpeed);
MenuItem(miAccTime, "Acc time", miResetVal, miMinSpeed, miSettings, Menu::NullItem, editAccTime);
MenuItem(miResetVal, "Raz all Values", miResetEEp, miAccTime, miSettings, Menu::NullItem, menuRAZ);
MenuItem(miResetEEp, "Reset EEprom", miBack3, miResetVal, miSettings, Menu::NullItem, menuReset);
MenuItem(miBack3, BACK, Menu::NullItem, miResetEEp, miSettings, Menu::NullItem, menuBack);

/*** TEST ***/
// Uncomment item "Winding" after
MenuItem(miWinding, "TEST", miMoves, Menu::NullItem, miExit,  Menu::NullItem, reworkTest);
/************/

// ----------------------------------------------------------------------------



#endif /* SRC_MENUSTRUCTURE_H_ */
