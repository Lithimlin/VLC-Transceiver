#include "TimerTwo.h"

TimerTwo Timer2;

ISR(TIMER2_COMPA_vect) {    //ISR on Compare Match A
  Timer2.isrCallback();
}

void TimerTwo::initialize(int frequency) {
  /** Prescalers:
    * http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061A.pdf#G3.1197678
    * Prescaler 32 : CS21, CS20
    * Prescaler 64 : CS22
    * Prescaler 128: CS22, CS20
    *
    * 500Hz: Prescaler 128, OCR2A: 249
    * 1kHz: Prescaler 64, OCR2A: 249
    * 2kHz: Prescaler 64, OCR2A: 124
    * 4kHz: Prescaler 32, OCR2A: 124
    *
    * OCR2A = [16,000,000Hz / (prescaler * desired interrupt frequency)] - 1 !< 256
    */
  uint8_t prescaler = 0;
  uint8_t value = 0;
  switch (frequency) {
    case 500: { //500Hz
      prescaler |= _BV(CS22);
      prescaler |= _BV(CS20);
      value = 249;
    } break;
    case 1000: { //1kHz
      prescaler |= _BV(CS22);
      value = 249;
    } break;
    case 2000: { //2kHz
      prescaler |= _BV(CS22);
      value = 124;
    } break;
    /*case 4000: { //4kHz
      prescaler |= _BV(CS21) |= _BV(CS20);
      value = 124;
    } break;*/ //Currently not usable as the filter controller is too slow
    default: { //1kHz
      prescaler |= _BV(CS22);
      value = 249;
    } break;
  }

  cli();
  TCCR2A = 0;               // set entire TCCR2A register to 0
  TCCR2B = 0;               // same for TCCR2B
  TCNT2  = 0;               // initialize counter value to 0

  TCCR2A  |=  _BV(WGM21);   // turn on CTC mode
  TCCR2B  |=  prescaler;    // set correct bits for needed prescaler
  OCR2A    =  value;        // set the needed time value
  sei();
}

void TimerTwo::attachInterrupt(void (*isr)()) {
  isrCallback = isr;
  TIMSK2 |= _BV(OCIE2A);    // (re)enable timer compare interrupt
}

void TimerTwo::detachInterrupt() {
  TIMSK2 &= ~_BV(OCIE2A);   // disable timer compare interrupt
}
