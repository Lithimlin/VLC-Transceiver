#include <Transciever.h>

Transciever::Transciever() {
  _transmitter.setPin(TRANSMITTER_PIN);
  _reciever.setPin(RECIEVER_PIN);
}

void Transciever::startTransmitter() {
  _transmitter.start();
}

void Transciever::stopTransmitter() {
  _transmitter.stop();
}

void Transciever::startReciever() {
  _reciever.start();
}

void Transciever::stopReciever() {
  _reciever.stop();
}

int Transciever::sendData(LEDBitmap image) {
  return _transmitter.sendData(image);
}

bool Transciever::lastTransmissionComplete() {
  return _transmitter.lastTransmissionComplete();
}

bool Transciever::transmitterIsStarted() {
  return _transmitter.isStarted();
}

bool Transciever::recieverIsStarted() {
  return _reciever.isStarted();
}

bool Transciever::isRecieving() {
  return _reciever.isRecieving();
}

bool Transciever::hadRecieverError() {
  return _reciever.hadError();
}

bool Transciever::receptionSuccessful() {
  return _reciever.receptionSuccessful();
}

LEDBitmap Transciever::getImage() {
  return _reciever.getImage();
}
