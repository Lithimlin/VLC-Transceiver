#ifndef TRANSCIEVER_H
#define TRANSCIEVER_H

#include <Arduino.h>

#include "Transmitter.h"
#include "Reciever.h"
#include "LEDBitmap.h"
#include "Constants.h"


class Transciever
{
  public:
    Transciever();
    Transciever(int frequency = 1000);
    virtual ~Transciever() {   };

  public: //methods
    void startTransmitter();
    void stopTransmitter();
    void startReciever();
    void stopReciever();
    int sendData(LEDBitmap image);
    bool lastTransmissionComplete();
    bool transmitterIsStarted();
    bool recieverIsStarted();
    bool isRecieving();
    bool hadRecieverError();
    bool receptionSuccessful();
    LEDBitmap getImage();

  private: //members
    Transmitter _transmitter;
    Reciever _reciever;
};

#endif
