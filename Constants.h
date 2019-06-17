#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>
#include <LEDBitmap.h>

#define PREAMBLE            0xA5  //The preamble for the transmition frame (10100101)
#define TRANSMIT_PERIOD     1000  //period within a manchester bit in µs (half the transmission period)
#define TRANSMIT_PERIOD_MS  TRANSMIT_PERIOD/1000
#define RECEIVE_TOLERANCE   (0.40)  //tolerance for the Receiver time measurement (between 0..1)

#define STRING              1
#define LEDBITMAP           2

const int MATRIX_PIN      = 2;
const int RECEIVER_PIN    = 3;
const int TRANSMITTER_PIN = 4;

// SLC for LED Matrix: 13
// (MISO for SPI: 12)
// DIN (MOSI) for LED Matrix: 11
// (SS for SPI: 10)

#define B1                7
#define B2                8
#define B3                9
#define B4                A1
#define LED_BLUE          5
#define LED_RED           6

const int BUTTON_1        = B1;
const int BUTTON_2        = B2;
const int BUTTON_3        = B3;
const int BUTTON_4        = B4;

const int YELLOW          = BUTTON_1;
const int RED             = BUTTON_2;
const int BLUE            = BUTTON_3;
const int GREEN           = BUTTON_4;

const int LED_STATUS_1    = LED_BLUE;
const int LED_STATUS_2    = LED_RED;

#endif
