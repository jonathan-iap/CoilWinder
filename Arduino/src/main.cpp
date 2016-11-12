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
#include "Display.h"

/*-----( Declare Constants )-----*/
/*-----( Declare objects )-----*/
Display display;
Coil CoilWinding;

/*-----( Declare Variables )-----*/

/* SETUP ---------------------------------------------------------------------*/
void setup()
{

  Serial.begin(115200);
  CoilWinding.begin();
  display.begin();

  Serial.print("\n\nbegin\n\n");

  delay(1000);
  pinMode(13, OUTPUT);
}
/*--(end setup )---*/

/* LOOP ----------------------------------------------------------------------*/
void loop()
{

  CoilWinding.setWinding(10.0,0.06,1000);
  CoilWinding.run();
  delay(2000);

  //  CoilWinding.oneLayer(10.0, CLOCK);
  //  delay(2000);
  //  CoilWinding.oneLayer(10.0, C_CLOCK);
  //  delay(2000);


  //CoilWinding.run();
}
/* --(end main loop )-- */

/* ( THE END ) */
