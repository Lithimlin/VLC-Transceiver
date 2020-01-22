#ifndef MATRIX_H
#define MATRIX_H

#include "Arduino.h"
#include <Max72xxPanel.h>
#include "LEDBitmap.h"

class Matrix : public Max72xxPanel {

  public:
    Matrix(byte csPin, byte hDisplays=1, byte vDisplays=1);

  public:
    void drawImage(int16_t x, int16_t y, LEDBitmap image, uint16_t color=HIGH, uint16_t bg=LOW);

};

#endif // MATRIX_H
