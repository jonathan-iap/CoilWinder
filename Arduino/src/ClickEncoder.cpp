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

#include "ClickEncoder.h"

ClickEncoder::ClickEncoder()
  : delta(0), last(0),
    button(Open)
{
  pinMode(ENC_PIN_A, INPUT_PULLUP);
  pinMode(ENC_PIN_B, INPUT_PULLUP);
  pinMode(ENC_PIN_BTN, INPUT_PULLUP);

  if (digitalRead(ENC_PIN_A) == LOW) {
    last = 3;
  }

  if (digitalRead(ENC_PIN_B) == LOW) {
    last ^=1;
  }
}

// ----------------------------------------------------------------------------
// call this every 1 millisecond via timer ISR
//
void ClickEncoder::service(void)
{
  unsigned long now = millis ();
    int8_t curr = 0;

    if (READ(ENC_PIN_A) == LOW)
      {
        curr = 3;
      }

    if (READ(ENC_PIN_B) == LOW)
      {
        curr ^= 1;
      }

    int8_t diff = last - curr;

    if (diff & 1)  // bit 0 = step
      {
        last = curr;
        delta += (diff & 2) - 1;   // bit 1 = direction (+/-)
      }

    static uint16_t keyDownTicks = 0;
    static unsigned long lastButtonCheck = 0;

    if ((now - lastButtonCheck) >= ENC_BUTTONINTERVAL) // checking button is sufficient every 10-30ms
      {
        lastButtonCheck = now;

        if (READ(ENC_PIN_BTN) == LOW)  // button is pressed
          {
            keyDownTicks++;
            button = Held;
          }
        else  // button is released
          {
            if (button == Held)
              button = Clicked;
          }
        keyDownTicks = 0;
      }
  }

// ----------------------------------------------------------------------------

int8_t ClickEncoder::getValue(void)
{
  int8_t val;

  cli();
  val = delta;
#if ENC_STEP == 2
  delta = val & 1;
#elif ENC_STEP == 4
  delta = val & 3;
#else
  delta = 0; // default to 1 step per notch
#endif
  sei();

#if ENC_STEP == 2
  val >>= 1;
#elif ENC_STEP == 4
  val >>= 2;
#endif

  int8_t r = 0;

  if (val < 0) r--;
  else if (val > 0) r++;

  return r;
}


ClickEncoder::Button ClickEncoder::getButton(void)
{
  ClickEncoder::Button ret = button;
  if (button != ClickEncoder::Held) {
    button = ClickEncoder::Open; // reset
  }
  return ret;
}
