// ----------------------------------------------------------------------------
// Rotary Encoder Driver
// Reworked by jonathan iapicco.
//
// Original author :
// (c) 2010 karl@pitrich.com
// (c) 2014 karl@pitrich.com
//
// Timer-based rotary encoder logic by Peter Dannegger
// http://www.mikrocontroller.net/articles/Drehgeber
// ----------------------------------------------------------------------------

#ifndef __have__ClickEncoder_h__
#define __have__ClickEncoder_h__

// ----------------------------------------------------------------------------

#include "Arduino.h"
#include "Fastio.h"
#include "Configuration.h"
// ----------------------------------------------------------------------------

#define ENC_BUTTONINTERVAL    10  // check button every x milliseconds, also debouce time

class ClickEncoder
{
public:
  typedef enum Button_e {
    Open = 0,
    Held,
    Clicked,
  } Button;

public:
  ClickEncoder();

  void service(void);
  int8_t getValue(void);
  Button getButton(void);

private:
  volatile int8_t delta;
  volatile int8_t last;
  volatile Button button;
};

// ----------------------------------------------------------------------------

#endif // __have__ClickEncoder_h__
