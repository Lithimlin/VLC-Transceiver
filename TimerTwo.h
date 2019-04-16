#ifndef TIMERTWO_H
#define TIMERTWO_H

#include <Arduino.h>

class TimerTwo
{
  public: //methods
    void initialize(int frequency = 1000);
    void attachInterrupt(void (*isr)());
    void detachInterrupt();
    void (*isrCallback)();
};

extern TimerTwo Timer2;

#endif
