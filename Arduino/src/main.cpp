/* Coil Winder program.

   History
   =======
   2016/OCT/13  - First version
*/

/*-----( Import needed libraries )-----*/
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Motor.h"
#include "Coil.h"
#include "Configuration.h"

/*-----( Declare Constants )-----*/
/*-----( Declare objects )-----*/
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

//Motor mA (M1_DIR, M1_STEP, M1_EN, M1_STEPS_PER_TR);
//Motor mB (M2_DIR, M2_STEP, M2_EN, M2_STEPS_PER_TR);

Coil CoilWinding();

/*-----( Declare Variables )-----*/

/* SETUP ---------------------------------------------------------------------*/
void setup()
{
  lcd.begin(16,2);

  Serial.begin(115200);

  Serial.print("\n\nbegin\n\n");
  delay(1000);
  pinMode(13, OUTPUT);

// ------- Quick 3 blinks of back-light  -------------
  for(int i = 0; i< 3; i++)
  {
    lcd.backlight();
    delay(250);
    lcd.noBacklight();
    delay(250);
  }
  lcd.backlight();
}
/*--(end setup )---*/

/* LOOP ----------------------------------------------------------------------*/
void loop()
{
// avant le bobinage
  //mA.init();
  //mB.init();
  CoilWinding.init(40.5,  0.1, 3500);

  // début du bobinage
  while(!CoilWinding.isFinished())
  {
      CoilWinding.go();
  }

}
/* --(end main loop )-- */

/* ( THE END ) */
