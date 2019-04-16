#ifndef LEDBITMAP_H
#define LEDBITMAP_H

#include <Arduino.h>

#define MAX_IMAGE_HEIGHT  8

class LEDBitmap
{

  public:
    LEDBitmap(uint8_t width, uint8_t height, int data[]);
    LEDBitmap(uint8_t width, uint8_t height);
    virtual ~LEDBitmap();

  public: //methods
    uint8_t* getBitmap();
    uint8_t getPixelValue(uint8_t x, uint8_t y);
    void setPixelValue(uint8_t x, uint8_t y, uint8_t value);
    void buildFromBytemap(uint8_t map[]);

    uint8_t getHeight() {
      return _height;
    };

    uint8_t getWidth() {
      return _width;
    };

    uint8_t getSize() {
      return _width * _height;
    };

    void clear();

  private: //members
    uint8_t _width;
    uint8_t _height;
    uint8_t *_data;

};

#endif
