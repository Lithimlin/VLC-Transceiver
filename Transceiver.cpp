#include <Transceiver.h>

Transceiver::Transceiver(int frequency) :
  _transmitter(frequency),
  _receiver(frequency)
{
  _transmitter.setPin(TRANSMITTER_PIN);
  _receiver.setPin(RECIEVER_PIN);
  _transmitter.setFrequency(frequency);
  _receiver.setFrequency(frequency);
}

void Transceiver::startTransmitter() {
  _transmitter.start();
}

void Transceiver::stopTransmitter() {
  _transmitter.stop();
}

void Transceiver::startReceiver() {
  _receiver.start();
}

void Transceiver::stopReceiver() {
  _receiver.stop();
}

int Transceiver::setFrequencies(int frequency) {
  frequency = _transmitter.setFrequency(frequency);
  _receiver.setFrequency(frequency);
  return frequency;
}

int Transceiver::setTransmitFrequency(int frequency) {
  return _transmitter.setFrequency(frequency);
}

int Transceiver::setReceiveFrequency(int frequency) {
  return _receiver.setFrequency(frequency);
}

int Transceiver::sendData(LEDBitmap &image) {
  return _transmitter.sendData(image);
}

int Transceiver::sendData(String &string) {
  return _transmitter.sendData(string);
}

bool Transceiver::lastTransmissionComplete() {
  return _transmitter.lastTransmissionComplete();
}

bool Transceiver::transmitterIsStarted() {
  return _transmitter.isStarted();
}

bool Transceiver::receiverIsStarted() {
  return _receiver.isStarted();
}

bool Transceiver::isRecieving() {
  return _receiver.isRecieving();
}

bool Transceiver::hadReceiverError() {
  return _receiver.hadError();
}

bool Transceiver::receptionSuccessful() {
  return _receiver.receptionSuccessful();
}

LEDBitmap Transceiver::getImage() {
  return _receiver.getImage();
}

String Transceiver::getString() {
  return _receiver.getString();
}

int Transceiver::getType() {
  return _receiver.getType();
}
