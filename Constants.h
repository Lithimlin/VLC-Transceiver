#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>
#include <LEDBitmap.h>

#define PREAMBLE          0xA5  //The preamble for the transmition frame (10100101)
#define TRANSMIT_PERIOD   1000  //period within a manchester bit in µs (half the transmission period)
#define TRANSMIT_PERIOD_MS  TRANSMIT_PERIOD/1000
#define RECIEVE_TOLERANCE 0.40  //tolerance for the Reciever time measurement (between 0..1)

const int MATRIX_PIN      = 2;
const int RECIEVER_PIN    = 3;
const int TRANSMITTER_PIN = 4;
// SLC for LED Matrix: 13
// DIN for LED Matrix: 12

#define S2    7
#define S3    8
#define S4    9
#define S5    10
#define S6    A1
#define BLUE  5
#define RED   6

const int BUTTON_UP       = S2;
const int BUTTON_DOWN     = S4;
const int BUTTON_LEFT     = S5;
const int BUTTON_RIGHT    = S6;
const int BUTTON_ENTER    = S3;
const int LED_STATUS_1    = BLUE;
const int LED_STATUS_2    = RED;

#endif
