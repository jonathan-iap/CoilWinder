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
Memory memory;


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

float WireSize;
float CoilLength;
float Turns;
float MaxSpeed;
float MinSpeed;
float AccDelay;

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

bool editWire(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      char test[BUFFSIZE_WIRE]={0};
      float result;
      memory.read(test, id_WIRESIZE);
      result = setting.setValue(test, BUFFSIZE_WIRE, MSG_WIRE_SIZE);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(result);
      delay(1000);
    }
  return true;
}

bool editLength(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      char test[BUFFSIZE_COIL]={0};
      float result;
      memory.read(test, id_COILLENGTH);
      result = setting.setValue(test, BUFFSIZE_COIL, MSG_COIL_LENGTH);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(result);
      delay(1000);
    }
  return true;
}

bool editTurns(const Menu::Action_t a)
{
  if (a == Menu::actionTrigger || a == Menu::actionDisplay)
    {
      char test[BUFFSIZE_TURNS]={0};
      float result;
      memory.read(test, id_TURNS);
      result = setting.setValue(test, BUFFSIZE_TURNS, MSG_TURNS);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(result);
      delay(1000);
    }
  return true;
}

bool menuReset(const Menu::Action_t a)
{
  if (a == Menu::actionDisplay)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("EEprom memory");
      lcd.setCursor(0,0);
      lcd.print("Reset");
      memory.reset();
      delay(1000);
    }
  return true;
}


// Framework for menu ---------------------------------------------------------

// Name, Label, Next, Previous, Parent, Child, Callback
// Menu 0
MenuItem(miExit, "", Menu::NullItem, Menu::NullItem, Menu::NullItem, miWinding, menuDummy);
// Menu 1 -> 3
MenuItem(miWinding, "Winding", miMoves, Menu::NullItem, miExit, miWireSize, menuDummy);
MenuItem(miMoves, "Moves", miSettings, miWinding, miExit, Menu::NullItem, menuDummy);
MenuItem(miSettings, "Settings", Menu::NullItem, miMoves, miExit, miReset, menuDummy);
// Sub-menu 1.1 -> 1.6
MenuItem(miWireSize, "1.Wire size", miCoilLength, Menu::NullItem, miWinding, Menu::NullItem, editWire);
MenuItem(miCoilLength, "2.Coil length", miTurns, miWireSize, miWinding, Menu::NullItem, editLength);
MenuItem(miTurns, "3.Turns", miStart, miCoilLength, miWinding, Menu::NullItem, editTurns);
MenuItem(miStart, "4.Start", miBack1, miTurns, miWinding, Menu::NullItem, menuDummy);
MenuItem(miBack1, "Back \1", Menu::NullItem, miStart, miWinding, Menu::NullItem, menuBack);
// Sub-menu 2.1 -> 2.?
// Sub-menu 3.1 -> 3.?
MenuItem(miReset, "Reset EEprom", miBack2, Menu::NullItem, miSettings, Menu::NullItem, menuReset);
MenuItem(miBack2, "Back \2", Menu::NullItem, miReset, miSettings, Menu::NullItem, menuBack);

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

  Serial.print("update value : ");
  Serial.print("WireSize : "); Serial.println(WireSize);
  Serial.print("CoilLength: "); Serial.println(CoilLength);
  Serial.print("Turns : ");   Serial.println(Turns);
  Serial.print("MinSpeed : ");   Serial.println(MinSpeed);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("check set buffer");

  memory.init();

  if( !memory.isSet() )
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

  Serial.print("update value : ");
  Serial.print("WireSize : "); Serial.println(WireSize);
  Serial.print("CoilLength: "); Serial.println(CoilLength);
  Serial.print("Turns : ");   Serial.println(Turns);
  Serial.print("MinSpeed : ");   Serial.println(MinSpeed);
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
