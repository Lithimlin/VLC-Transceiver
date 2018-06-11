#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <Arduino.h>
#include "TimerTwo.h"
#include "Constants.h"
#include "LEDBitmap.h"

#define MAX_BYTE_FRAME_SIZE 10
#define MAX_BIT_FRAME_SIZE  MAX_BYTE_FRAME_SIZE*8

class Transmitter
{
  public:
    Transmitter();
    virtual ~Transmitter() {  };

  public: //methods
    int sendData(LEDBitmap image);
    void transmitBit();
    void start();
    void stop();
    bool lastTransmissionComplete();
    bool isStarted() { return _active; };
    void setPin(int pin);
    int getPin();

  private: //members
    uint8_t _state;
    uint8_t _frame[MAX_BIT_FRAME_SIZE];
    uint16_t _frameSize;
    uint16_t _pos;
    uint8_t _manHalf;
    int _pin;
    bool _busy;
    bool _active;
    static Transmitter *_instance = nullptr;

    int prepFrame(LEDBitmap image);
    uint8_t buildBitFrame(uint8_t* frame, uint8_t size, uint8_t* outFrame, size_t outFrameSize);
    static void _transmitBit();

};

#endif
