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

#define BACK	"Back" ICONBACK

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

bool setHome(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      char tmp_buffDistance[] = INIT_MOV_CAR;
      float tmp_distance = 0.00;

      setting.actionMenu(id_HOME, tmp_buffDistance, &tmp_distance);
    }
  return true;
}

bool editWire(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.actionMenu(id_WIRESIZE);
    }
  return true;
}

bool editLength(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.actionMenu(id_COILLENGTH);
    }
  return true;
}

bool editTurns(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.actionMenu(id_TURNS);
    }
  return true;
}

bool runWinding(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.actionMenu(id_NEW);
    }
  return true;
}

bool runResume(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.actionMenu(id_RESUME);
    }
  return true;
}

bool runSaved(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.actionMenu(id_RESUME_SAVE);
    }
  return true;
}

bool editMaxSpeed(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.actionMenu(id_MAX_SPEED);
    }
  return true;
}

bool editMinSpeed(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.actionMenu(id_MIN_SPEED);
    }
  return true;
}

bool editAccTime(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.actionMenu(id_ACC_DELAY);
    }
  return true;
}

bool editAccIncr(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.actionMenu(id_ACC_INCR);
    }
  return true;
}

bool menuMovCarriage(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      char tmp_buffDistance[] = INIT_MOV_CAR;
      float tmp_distance = 0.00;

      setting.actionMenu(id_MOVE_CARRIAGE, tmp_buffDistance, &tmp_distance);
    }
  return true;
}

bool menuMovCoil(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      char tmp_buffDistance[] = INIT_MOV_COIL;
      float tmp_distance = 0.00;

      setting.actionMenu(id_MOVE_COIL, tmp_buffDistance, &tmp_distance);
    }
  return true;
}

bool menuReset(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.actionMenu(id_RESET);
    }
  return true;
}

bool menuRAZ(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      setting.actionMenu(id_RAZ);
    }
  return true;
}


// Framework for menu ---------------------------------------------------------

// Name, Label, Next, Previous, Parent, Child, Callback
// Menu 0
MenuItem(miHome, "", Menu::NullItem, Menu::NullItem, Menu::NullItem, miSetWinding, menuDummy);

// Menu 1 -> 4
MenuItem(miSetWinding, "Set winding",   miWinding,      Menu::NullItem, miHome, miSetHome	, menuDummy);
MenuItem(miWinding,    "Start winding", miMoves,        miSetWinding,   miHome, miStart		, menuDummy);
MenuItem(miMoves,      "Moves",         miSettings,     miWinding,      miHome, miMovCarriage	, menuDummy);
MenuItem(miSettings,   "Settings",      Menu::NullItem, miMoves,        miHome, miMaxSpeed	, menuDummy);

// Sub-menu 1.1 -> 1.7
MenuItem(miSetHome,    "Set home",      miWireSize,     Menu::NullItem, miSetWinding, Menu::NullItem, setHome);
MenuItem(miWireSize,   "Wire size",     miCoilLength,   miSetHome,      miSetWinding, Menu::NullItem, editWire);
MenuItem(miCoilLength, "Coil length",   miTurns,        miWireSize,     miSetWinding, Menu::NullItem, editLength);
MenuItem(miTurns,      "Turns",         miWinSense,     miCoilLength,   miSetWinding, Menu::NullItem, editTurns);
MenuItem(miWinSense,   "Winding sense", miCarSense,     miTurns,        miSetWinding, Menu::NullItem, menuDummy);
MenuItem(miCarSense,   "Start sense",   miBack1,        miWinSense,     miSetWinding, Menu::NullItem, menuDummy);
MenuItem(miBack1,      BACK,            Menu::NullItem, miCarSense,     miSetWinding, Menu::NullItem, menuBack);

// Sub-menu 2.1 -> 2.4
MenuItem(miStart,  "Start new",      miResume,       Menu::NullItem, miWinding, Menu::NullItem, runWinding);
MenuItem(miResume, "Resume current", miSaved,        miStart,        miWinding, Menu::NullItem, runResume);
MenuItem(miSaved,  "Resume saved",   miBack2,        miResume,       miWinding, Menu::NullItem, runSaved);
MenuItem(miBack2,  BACK,             Menu::NullItem, miSaved,        miWinding, Menu::NullItem, menuBack);

// Sub-menu 3.1 -> 3.3
MenuItem(miMovCarriage, "Move carriage", miMovCoil,      Menu::NullItem, miMoves, Menu::NullItem, menuMovCarriage);
MenuItem(miMovCoil,     "Move coil",     miBack3,        miMovCarriage,  miMoves, Menu::NullItem, menuMovCoil);
MenuItem(miBack3,       BACK,            Menu::NullItem, miMovCoil,      miMoves, Menu::NullItem, menuBack);

// Sub-menu 4.1 -> 4.6
MenuItem(miMaxSpeed, "Max speed",      miMinSpeed,     Menu::NullItem, miSettings, Menu::NullItem, editMaxSpeed);
MenuItem(miMinSpeed, "Min speed",      miAccTime,      miMaxSpeed,     miSettings, Menu::NullItem, editMinSpeed);
MenuItem(miAccTime,  "Acc time",       miAccIncr,      miMinSpeed,     miSettings, Menu::NullItem, editAccTime);
MenuItem(miAccIncr,  "Acc increment",  miResetEEp,     miAccTime,      miSettings, Menu::NullItem, editAccIncr);
MenuItem(miResetEEp, "Reset EEprom",   miResetVal,     miAccIncr,      miSettings, Menu::NullItem, menuReset);
MenuItem(miResetVal, "Raz all Values", miBack4,        miResetEEp,     miSettings, Menu::NullItem, menuRAZ);
MenuItem(miBack4,    BACK,             Menu::NullItem, miResetVal,     miSettings, Menu::NullItem, menuBack);

// ----------------------------------------------------------------------------



#endif /* SRC_MENUSTRUCTURE_H_ */
