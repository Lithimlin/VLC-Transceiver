#include "LEDBitmap.h"

LEDBitmap::LEDBitmap(uint8_t const width, uint8_t const height) {
  _width = width;
  if(width == 0)  _width = 8;
  _height = height;
  if(height == 0) _height = 8;
  _data = (uint8_t*)malloc(_height*sizeof(uint8_t));
}

LEDBitmap::LEDBitmap(uint8_t width, uint8_t height, int data[]) {
  _width = width;
  if(width == 0)  _width = 8;
  _height = height;
  if(height == 0) _height = 8;
  _data = (uint8_t*)malloc(_height*sizeof(uint8_t));

  for (uint8_t h = 0; h < height; h++) {
    uint8_t binary = 0x00;
    for (uint8_t w = 0; w < width; w++) {
      if(data[w + width*h] >= 1)
        binary |= _BV(8 - (w+1));
      else
        binary &= ~_BV(8 - (w+1));
    }
    _data[h] = binary;
  }
}

LEDBitmap::~LEDBitmap(){
  free(_data);
}

uint8_t LEDBitmap::getPixelValue(uint8_t x, uint8_t y) {
  uint8_t value = _data[y] >> (8 - (x+1));
  return (value && 0x01);
}

void LEDBitmap::setPixelValue(uint8_t x, uint8_t y, uint8_t value) {
  _data[y] &=  ~_BV(8 - (x+1));
  if (value >= 1) {
    _data[y] |= _BV(8 - (x+1));
  }
}

void LEDBitmap::buildFromBytemap(uint8_t map[]) {
  for(uint8_t h = 0; h < _height; h++) {
    _data[h] = map[h];
  }
}

uint8_t* LEDBitmap::getBitmap() {
  return _data;
}

void LEDBitmap::clear() {
  for (uint8_t h = 0; h < _height; h++) {
    for (uint8_t w = 0; w < _width; w++) {
      _data[w + _width*h] = 0;
    }
  }
}
