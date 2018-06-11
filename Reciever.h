#ifndef RECIEVER_H
#define RECIEVER_H
#include <Arduino.h>
#include "SimpleFIFO.h"
#include "LEDBitmap.h"
#include "Constants.h"

#define T1 TRANSMIT_PERIOD
#define T2 T1*2
#define TOLERANCE T1*RECIEVE_TOLERANCE

#define T1_LOW (T1 - TOLERANCE)
#define T1_HIGH (T1 + TOLERANCE)
#define T2_LOW (T2 - TOLERANCE)
#define T2_HIGH (T2 + TOLERANCE)

#define IN_T1(t) ((t > T1_LOW) && (t < T1_HIGH))
#define IN_T2(t) ((t > T2_LOW) && (t < T2_HIGH))

#define IMAGE_HEIGHT  8
#define IMAGE_WIDTH   8

class Reciever
{
  public:
    Reciever();
    virtual ~Reciever() {   };

  public: //methods
    void setPin(int pin) { _pin = pin; };
    int getPin() { return _pin; }
    void start();
    void stop();
    bool isRecieving() { return _recieving; };
    bool isStarted() { return _started; };
    bool hadError();
    bool receptionSuccessful();
    LEDBitmap getImage();



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
      FETCH_DATA,
      DATA_COMPLETE,
      RECEPTION_FINISHED,
      ERROR
    };

    bool _started;
    bool _recieving;
    bool _hadError;
    bool _success;
    int _pin;
    unsigned long _lastTime;
    uint8_t _data[12];
    uint8_t _recievedByteCtr;
    uint8_t _value;
    SimpleFIFO _bitBuffer;
    LEDBitmap _image;
    State _state;
    ProcessState _processState;
    static Reciever* _instance = nullptr;

    void process(uint8_t value);
    void pushValue(uint8_t value);
    void checkPreamble();
    bool checksumCorrect();
    void buildImage();
    void handleError();
    void clear();
    void switchState();
    static void _switchState();

};

#endif
