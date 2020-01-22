#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H

#include <Arduino.h>

#include "Transmitter.h"
#include "Receiver.h"
#include "LEDBitmap.h"
#include "Constants.h"


class Transceiver
{
  public:
    Transceiver(int frequency = 1000);
    virtual ~Transceiver() {   };

  public: //methods
    void startTransmitter();
    void stopTransmitter();
    void startReceiver();
    void stopReceiver();
    int setFrequencies(int frequency = 1000);
    int setTransmitFrequency(int frequency = 1000);
    int setReceiveFrequency(int frequency = 1000);
    int sendData(LEDBitmap &image);
    int sendData(String &string);
    bool lastTransmissionComplete();
    bool transmitterIsStarted();
    bool receiverIsStarted();
    bool isReceiving();
    bool hadReceiverError();
    bool receptionSuccessful();
    LEDBitmap getImage();
    String getString();
    int getType();

    bool handleReception(Matrix* matrix);

  private: //members
    Transmitter _transmitter;
    Receiver _receiver;
};

#endif
