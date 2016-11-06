/* Coil Winder program.

   History
   =======
   2016/OCT/13  - First version
 */

/*-----( Import needed libraries )-----*/
#include "Arduino.h"
#include "Motor.h"
#include "Coil.h"
#include "Configuration.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "Display.h"

/*-----( Declare Constants )-----*/
/*-----( Declare objects )-----*/
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
Display display;
Coil CoilWinding;

/*-----( Declare Variables )-----*/

/* SETUP ---------------------------------------------------------------------*/
void setup()
{
  lcd.begin(16,2);
  Serial.begin(115200);
  CoilWinding.begin();

  Serial.print("\n\nbegin\n\n");

  display.home();

  delay(1000);
  pinMode(13, OUTPUT);
}
/*--(end setup )---*/

/* LOOP ----------------------------------------------------------------------*/
void loop()
{

  CoilWinding.setWinding(20,0.8,100);
  CoilWinding.run();
  delay(2000);
  //CoilWinding.run();
}
/* --(end main loop )-- */

/* ( THE END ) */
