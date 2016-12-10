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
#include "MenuSettings.h"
#include "Save.h"


// Declare objects ------------------------------------------------------------
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
ClickEncoder Encoder(ENC_PIN_A, ENC_PIN_B, ENC_PIN_SW, ENC_STEP);
Menu::Engine *engine;
Display display(&lcd);
Coil CoilWinding;
Setting setting(&lcd, &Encoder);

// Global Variables -----------------------------------------------------------
namespace State
{
  typedef enum SystemMode_e
  {
    None      	= 0,
    Default   	= 1,
    Move	= 2,
    Edit	= 3,
    Back	= 4
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
void timerIsr(void)
{
  Encoder.service();
}

void renderMenuItem(const Menu::Item_t *mi, uint8_t pos)
{
  lcd.setCursor(0, pos);

  // Print icon before current item else blank.
  engine->currentItem == mi ? lcd.write((uint8_t)IconEnter) : lcd.write(20); ;

  // Print label item
  lcd.print(engine->getLabel(mi));

  // mark items that have children
  if (engine->getChild(mi) != &Menu::NullItem)
    {
      lcd.write(20);
      lcd.write((uint8_t)IconRight);
    }
  // clear characters after items.
  lcd.print("      ");
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
      systemState = State::Back;
    }
  return true;
}

bool menuEdit(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      char testName[] = "valeur de test";
      char test[] = "00.00";
      float result;
      result = setting.setValue(test, sizeof(test), testName);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(result);
      delay(1000);
      //systemState = State::Edit;
    }
  return true;
}

// Framework for menu ---------------------------------------------------------

// Name, Label, Next, Previous, Parent, Child, Callback
// Menu 0
MenuItem(miExit, "", Menu::NullItem, Menu::NullItem, Menu::NullItem, miWinding, menuDummy);
// Menu 1 -> 3
MenuItem(miWinding, "Winding", miMoves, Menu::NullItem, miExit, miWireSize0, menuDummy);
MenuItem(miMoves, "Moves", miSettings, miWinding, miExit, Menu::NullItem, menuDummy);
MenuItem(miSettings, "Settings", Menu::NullItem, miMoves, miExit, Menu::NullItem, menuDummy);
// Sub-menu 1.1 -> 1.6
MenuItem(miWireSize0, "1.Wire size", miCoilLength0, Menu::NullItem, miWinding, Menu::NullItem, menuEdit);
MenuItem(miCoilLength0, "2.Coil length", miTurns0, miWireSize0, miWinding, Menu::NullItem, menuEdit);
MenuItem(miTurns0, "3.Turns", miStart0, miCoilLength0, miWinding, Menu::NullItem, menuEdit);
MenuItem(miStart0, "4.Start", miBack0, miTurns0, miWinding, Menu::NullItem, menuDummy);
MenuItem(miBack0, "Back \1", Menu::NullItem, miStart0, miWinding, Menu::NullItem, menuBack);

// ----------------------------------------------------------------------------



/* SETUP ---------------------------------------------------------------------*/
void setup()
{
  // Winding function
  CoilWinding.begin();

  // For rotary encoder
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);

  // Menu begin
  engine = new Menu::Engine(&Menu::NullItem);

  // Pin initialization
  pinMode(13, OUTPUT);

  // Lcd initialization
  display.begin();

  // Debug section
#ifdef DEBUG
  Serial.begin(BAUDRATE);

  Serial.print("\n\nbegin\n\n");
  delay(1000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("check set buffer");

  Memory mem;
  mem.init();

  if( !mem.isSet() )
    {
      lcd.setCursor(0,1);
      lcd.print("not set");
    }
  else
    {
      lcd.setCursor(0,1);
      lcd.print("is set");
    }
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
    // First use print software version
    case State::Default :
      {
	display.version();
	break;
      }
    case State::Back :
      {
	systemState = State::Move;
	engine->navigate(engine->getParent());
	updateMenu = true;
	break;
      }
  }

}

/* ( THE END ) */
