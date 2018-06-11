#include "TimerTwo.h"

TimerTwo Timer2;

ISR(TIMER2_COMPA_vect) {    //ISR on Compare Match A
  Timer2.isrCallback();
}

void TimerTwo::initialize() {
  cli();
  TCCR2A = 0;               // set entire TCCR2A register to 0
  TCCR2B = 0;               // same for TCCR2B
  TCNT2  = 0;               // initialize counter value to 0

  TCCR2A  |=  _BV(WGM21);   // turn on CTC mode
  TCCR2B  |=  _BV(CS22);    // Set CS22 bit for 64 prescaler
  OCR2A    =  249;          // = (16*10^6) / (1000*64) - 1 (must be <256)
  sei();
}

void TimerTwo::attachInterrupt(void (*isr)()) {
  isrCallback = isr;
  TIMSK2 |= _BV(OCIE2A);    // (re)enable timer compare interrupt
}

void TimerTwo::detachInterrupt() {
  TIMSK2 &= ~_BV(OCIE2A);   // disable timer compare interrupt
}
