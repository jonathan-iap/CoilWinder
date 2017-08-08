/* Coil Winder program.

   History
   =======
   2016/OCT/13  - First version
 */

// libraries ------------------------------------------------------------------
#include "Arduino.h"
#include "Configuration.h"
#include "Display.h"
#include "ClickEncoder.h"
#include "TimerOne.h"
#include "MenuStructure.h"
#include "MsTimer2.h"

// Declare objects ------------------------------------------------------------
ClickEncoder Encoder;
Menu::Engine *engine;
Display display;


// Global Variables -----------------------------------------------------------
uint8_t systemState = state_DEFAULT;
uint8_t previousSystemState = state_NONE;
uint8_t menuItemsVisible = LCD_LINES;
int16_t encMovement;
int16_t encAbsolute;
int16_t tmpValue = 0;

bool updateMenu = false;
bool lastEncoderAccelerationState = true;


// Functions ------------------------------------------------------------------
void timerEncoder(void)
{
	Encoder.service();
}



/* SETUP ---------------------------------------------------------------------*/
void setup()
{
	// Debug section
#ifdef DEBUG
	Serial.begin(BAUDRATE);

	Serial.print("\n\nbegin\n\n");
	delay(1000);
#endif

	// Lcd initialization
	display.begin();

	// For rotary encoder
	MsTimer2::set(1, timerEncoder); // 1ms
	MsTimer2::start();

	// Menu begin
	engine = new Menu::Engine(&Menu::NullItem);

	// Pin initialization
	pinMode(13, OUTPUT);
}

/* LOOP ----------------------------------------------------------------------*/
void loop()
{
	// Display section.
	switch(systemState)
	{
	// First use print software version
	case state_DEFAULT :
	{
		display.version();
		break;
	}
	case state_BACK :
	{
		systemState = state_MOVE;
		engine->navigate(engine->getParent());
		updateMenu = true;
		break;
	}
	}

	// handle encoder
	encMovement = Encoder.getValue();
	if (encMovement)
	{
		encAbsolute += encMovement;

		switch(systemState)
		{
		case state_MOVE :
		{
			engine->navigate((encMovement > 0) ? engine->getNext() : engine->getPrev());
			updateMenu = true;
			break;
		}

		case state_EDIT :
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
		if(systemState == state_MOVE)
		{
			engine->invoke();
			updateMenu = true;
		}
		else if(systemState == state_EDIT)
		{
			systemState = state_MOVE;
			engine->navigate(engine->getParent());
			updateMenu = true;
		}
		else
		{
			// enter settings menu
			engine->navigate(&miSetWinding);

			systemState = state_MOVE;
			previousSystemState = systemState;
			updateMenu = true;
		}
		break;
	}
	case ClickEncoder::Held:{break;}
	case ClickEncoder::Open:{break;}
	}

	// update LCD
	if (updateMenu)
	{
		updateMenu = false;

		if (!encMovement)
		{
			// clear menu on child/parent navigation
			display.clear();
		}
		// render the menu
		engine->render(renderMenuItem, menuItemsVisible);
	}
}

/* ( THE END ) */
