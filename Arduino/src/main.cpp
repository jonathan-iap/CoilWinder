/* Coil Winder program.

   History
   =======
   2016/OCT/13  - First version
 */

/*-----( Import needed libraries )-----*/
#include "Arduino.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "Motor.h"
#include "Coil.h"
#include "Configuration.h"

/*-----( Declare Constants )-----*/
/*-----( Declare objects )-----*/
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

Coil CoilWinding;

/*-----( Declare Variables )-----*/

/* SETUP ---------------------------------------------------------------------*/
void setup()
{
  lcd.begin(16,2);

  Serial.begin(115200);

  CoilWinding.begin();

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
 CoilWinding.setWinding(20,0.8,100);
 CoilWinding.run();
 delay(2000);
  //CoilWinding.run();
}
/* --(end main loop )-- */

/* ( THE END ) */
