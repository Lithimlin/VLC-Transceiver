#include <Transciever.h>

Transciever::Transciever(int frequency) :
  _transmitter(frequency),
  _reciever(frequency)
{
  _transmitter.setPin(TRANSMITTER_PIN);
  _reciever.setPin(RECIEVER_PIN);
  _transmitter.setFrequency(frequency);
  _reciever.setFrequency(frequency);
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

int Transciever::setFrequencies(int frequency) {
  frequency = _transmitter.setFrequency(frequency);
  _reciever.setFrequency(frequency);
  return frequency;
}

int Transciever::setTransmitFrequency(int frequency) {
  return _transmitter.setFrequency(frequency);
}

int Transciever::setRecieveFrequency(int frequency) {
  return _reciever.setFrequency(frequency);
}

int Transciever::sendData(LEDBitmap &image) {
  return _transmitter.sendData(image);
}

int Transciever::sendData(String &string) {
  return _transmitter.sendData(string);
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

String Transciever::getString() {
  return _reciever.getString();
}

int Transciever::getType() {
  return _reciever.getType();
}
