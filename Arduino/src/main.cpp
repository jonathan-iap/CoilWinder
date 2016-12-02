/* Coil Winder program.

   History
   =======
   2016/OCT/13  - First version
 */

// libraries ------------------------------------------------------------------
#include "Arduino.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "Coil.h"
#include "Configuration.h"
#include "Display.h"
#include "ClickEncoder.h"
#include "TimerOne.h"
#include "Menu.h"

// Declare objects ------------------------------------------------------------

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
Display display;
Coil CoilWinding;
Menu::Engine *engine;
ClickEncoder Encoder(ENC_PIN_A, ENC_PIN_B, ENC_PIN_SW, ENC_STEP);
void timerIsr(void)
{
  Encoder.service();
}

// Global Variables -----------------------------------------------------------
namespace State
{
  typedef enum SystemMode_e
  {
    None      	= 0,
    Default   	= 1,
    Move	= 2,
    Edit	= 3
  } SystemMode;
};

uint8_t systemState = State::Default;
uint8_t previousSystemState = State::None;
uint8_t menuItemsVisible = LCD_LINES;
int16_t encMovement;
int16_t encAbsolute;
int16_t encLastAbsolute = -1;
int16_t tmpValue = 0;
bool updateMenu = false;
bool lastEncoderAccelerationState = true;


// Functions ------------------------------------------------------------------
void renderMenuItem(const Menu::Item_t *mi, uint8_t pos)
{
  lcd.setCursor(0, pos);

  // cursor
  if (engine->currentItem == mi)
    {
      lcd.write((uint8_t)IconEnter);
    }
  else
    {
      lcd.write(20); // space
    }

  lcd.print(engine->getLabel(mi));

  // mark items that have children
  if (engine->getChild(mi) != &Menu::NullItem)
    {
      lcd.write(20);
      lcd.write((uint8_t)IconRight);
    }

  lcd.print("      "); // clear characters after items.
}

// CallBacks -------------------------------------------------------------------
bool menuExit(const Menu::Action_t a)
{
  Encoder.setAccelerationEnabled(lastEncoderAccelerationState);
  systemState = State::Default;
  return true;
}

bool menuDummy(const Menu::Action_t a)
{
  return true;
}

bool menuBack(const Menu::Action_t a)
{
  if (a == Menu::actionDisplay)
    {
      engine->navigate(engine->getParent(engine->getParent()));
    }
  return true;
}

bool menuEdit(const Menu::Action_t a)
{
  if (a == Menu::actionDisplay)
    {
      lcd.clear();
      systemState = State::Edit;
    }
  return true;
}

// Framework for menu ---------------------------------------------------------

// Name, Label, Next, Previous, Parent, Child, Callback
// Menu 0
MenuItem(miExit, "", Menu::NullItem, Menu::NullItem, Menu::NullItem, miWinding, menuExit);
// Menu 1 -> 3
MenuItem(miWinding, "Winding", miMoves, Menu::NullItem, miExit, miWireSize0, menuDummy);
MenuItem(miMoves, "Moves", miSettings, miWinding, miExit, Menu::NullItem, menuDummy);
MenuItem(miSettings, "Settings", Menu::NullItem, miMoves, miExit, Menu::NullItem, menuDummy);
// Sub-menu 1.1 -> 1.6
MenuItem(miWireSize0, "1.Wire size", miCoilLength0, Menu::NullItem, miWinding, miSetWireSize0, menuEdit);
MenuItem(miCoilLength0, "2.Coil length", miTurns0, miWireSize0, miWinding, miSetCoilLength0, menuEdit);
MenuItem(miTurns0, "3.Turns", miStart0, miCoilLength0, miWinding, miSetTurns0, menuEdit);
MenuItem(miStart0, "4.Start", miBack0, miTurns0, miWinding, Menu::NullItem, menuDummy);
MenuItem(miBack0, "Back \1", Menu::NullItem, miStart0, miWinding, Menu::NullItem, menuBack);
// Sub-Menu 1.1.1 -> 1.1.4
MenuItem(miSetWireSize0, "Wire size in mm", Menu::NullItem, Menu::NullItem, miWireSize0, Menu::NullItem, menuDummy);
MenuItem(miSetCoilLength0, "Length of coil mm", Menu::NullItem, Menu::NullItem, miCoilLength0, Menu::NullItem, menuDummy);
MenuItem(miSetTurns0, "Number of turns", Menu::NullItem, Menu::NullItem, miTurns0, Menu::NullItem, menuDummy);


// ----------------------------------------------------------------------------



/* SETUP ---------------------------------------------------------------------*/
void setup()
{
  // For debug
#ifdef DEBUG
  Serial.begin(BAUDRATE);
#endif

  // Winding function
  CoilWinding.begin();

  // For rotary encoder
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);

  // Menu begin
  engine = new Menu::Engine(&Menu::NullItem);

  // Pin initialization
  pinMode(13, OUTPUT);

  // Lcd screen
  display.begin();

#ifdef DEBUG
  Serial.print("\n\nbegin\n\n");
  delay(1000);
#endif
}

/* LOOP ----------------------------------------------------------------------*/
void loop()
{
  // handle encoder
  encMovement = Encoder.getValue();
  if (encMovement)
    {
      encAbsolute += encMovement;

      switch(systemState)
      {
	case State::Move :
	  {
	    engine->navigate((encMovement > 0) ? engine->getNext() : engine->getPrev());
	    updateMenu = true;
	    break;
	  }

	case State::Edit :
	  {
	    (encMovement > 0) ? tmpValue++ : tmpValue--;
	    updateMenu = true;
	    break;
	  }
      }
    }

  // handle button
  switch (Encoder.getButton())
  {

    case ClickEncoder::Clicked:
      {
	// Enter in item
	if(systemState == State::Move)
	  {
	    engine->invoke();
	    updateMenu = true;
	  }
	else if(systemState == State::Edit)
	  {
	    systemState = State::Move;
	    engine->navigate(engine->getParent());
	    updateMenu = true;
	  }
	else
	  {
	    // enter settings menu
	    // disable acceleration, reset in menuExit()
	    lastEncoderAccelerationState = Encoder.getAccelerationEnabled();
	    Encoder.setAccelerationEnabled(false);

	    engine->navigate(&miWinding);

	    systemState = State::Move;
	    previousSystemState = systemState;
	    updateMenu = true;
	  }
	break;
      }
    case ClickEncoder::DoubleClicked:
      {
	// Back to previous item
	if (systemState == State::Move)
	  {
	    engine->navigate(engine->getParent());
	    updateMenu = true;
	  }
	break;
      }
  }

  if (updateMenu)
    {
      updateMenu = false;

      if (!encMovement)
	{
	  // clear menu on child/parent navigation
	  lcd.clear();
	}
      // render the menu
      engine->render(renderMenuItem, menuItemsVisible);
    }

  // Display section.
  switch(systemState)
  {
    case State::Default :
      {
	if (systemState != previousSystemState)
	  {
	    previousSystemState = systemState;
	    encLastAbsolute = -999;

	    lcd.setCursor(0, 0);
	    lcd.print("Coil Winder v1.0");
	    lcd.setCursor(0,1);
	    lcd.print("click to start");
	  }
	break;
      }

    case State::Edit :
      {
	if (encAbsolute != encLastAbsolute)
	  {
	    lcd.setCursor(5, 1);
	    lcd.print(tmpValue);
	  }
	break;
      }
  }

}

/* ( THE END ) */
