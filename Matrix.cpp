#include "Matrix.h"

Matrix::Matrix(byte csPin, byte hDisplays, byte vDisplays) : Max72xxPanel(csPin, hDisplays, vDisplays) {}

void Matrix::drawImage(int16_t x, int16_t y, LEDBitmap image, uint16_t color, uint16_t bg) {
  Max72xxPanel::drawBitmap(x, y, image.getBitmap(), image.getWidth(), image.getHeight(), color, bg);
}
