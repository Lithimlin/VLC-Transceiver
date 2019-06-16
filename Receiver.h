#ifndef RECEIVER_H
#define RECEIVER_H
#include <Arduino.h>
#include "SimpleFIFO.h"
#include "LEDBitmap.h"
#include "Constants.h"

#define T1 _transmit_period
#define T2 T1*2
#define TOLERANCE T1*RECEIVE_TOLERANCE

#define T1_LOW (T1 - TOLERANCE)
#define T1_HIGH (T1 + TOLERANCE)
#define T2_LOW (T2 - TOLERANCE)
#define T2_HIGH (T2 + TOLERANCE)

#define IN_T1(t) ((t > T1_LOW) && (t < T1_HIGH))
#define IN_T2(t) ((t > T2_LOW) && (t < T2_HIGH))

typedef union u_Data {
  LEDBitmap bitmap;
  String string;
  u_Data() : bitmap(0,0) {};
  ~u_Data() {bitmap.~LEDBitmap();};
} Data;

class Receiver
{
  public:
    Receiver(int frequency = 1000);
    virtual ~Receiver(){};

  public: //methods
    void setPin(int pin) { _pin = pin; };
    int getPin() { return _pin; }
    void start();
    void stop();
    int setFrequency(int frequency);
    bool isRecieving() { return _recieving; };
    bool isStarted() { return _started; };
    bool hadError();
    bool receptionSuccessful();
    LEDBitmap getImage();
    String getString();
    int getType();



  private: //members
    enum class State{
      RX_IDLE,
      RX_START_RECEPTION,
      RX_BIT_REPITITION1,
      RX_BIT_REPITITION2,
      RX_BIT_ALTERATION,
      RX_ERROR
    };

    enum class ProcessState{
      FETCH_PREAMBLE,
      FETCH_TYPE,
      FETCH_SIZE,
      FETCH_DATA,
      PROCESS_DATA,
      RECEPTION_FINISHED,
      ERROR
    };

    uint16_t _transmit_period;
    bool _started;
    bool _recieving;
    bool _hadError;
    bool _success;
    int _pin;
    unsigned long _lastTime;
    struct LastReception {
      uint8_t type;
      Data data = {};
    } _lastReception;
    struct CurrentReception {
      uint8_t type;
      uint8_t size[2];
    } _currentReception;
    uint8_t _data[260];
    uint16_t _receivedByteCtr;
    uint8_t _value;
    SimpleFIFO _bitBuffer;
    State _state;
    ProcessState _processState;
    static Receiver* _instance = nullptr;

    void process(uint8_t value);
    void pushValue(uint8_t value);
    void checkPreamble();
    void readType();
    void readSize();
    bool checksumCorrect();
    void buildString();
    void buildImage();
    void handleError();
    void clear();
    void switchState();
    static void _switchState();

};

#endif
