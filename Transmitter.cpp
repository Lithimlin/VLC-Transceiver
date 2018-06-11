#include <Arduino.h>
#include <Transmitter.h>

Transmitter::Transmitter()
{
  _pin = 0;
  _pos = 0;
  _manHalf = 0;
  _busy = false;
  _state = 0;
  _active = false;
  Timer2.initialize();
}

Transmitter* Transmitter::_instance;

/**
  * sends a LEDBitmap of a width of 8 and a height up to 8
  * via the specified LED pin.
  *
  * @param image  The image that is to be transmitted.
  *
  * @return 1 if the transmission was successful,
  *         -1 if there is already a transmission active.
  *         -2 if no transmission pin has been specified.
  */
int Transmitter::sendData(LEDBitmap image) {
  return prepFrame(image);
}

/**
  * prepares a LEDBitmap for transmission on the specified LED pin.
  *
  * @param image  The image that is to be prepared.
  *
  * @return 1 if the transmission was successful,
  *         -1 if there is already a transmission active.
  *         -2 if no transmission pin has been specified.
  */
int Transmitter::prepFrame(LEDBitmap image) {
  if(_busy) return -1; //no Data can be sent right now since the transmitter is already busy
  if(!_pin) return -2;
  /** construct the frame that is to be transmitted
    *
    * frame structure is
    * 1 byte preamble (0xA5) | 8 byte payload | checksum
    *
    * the image will be transmitted in rows
    * with one row being one byte
    *  1  2  3  4  5  6  7  8   <-- 1st byte
    *  9 10 11 12 13 14 15 16   <-- 2nd byte
    * 17 18 19 20 21 22 23 24   <-- 3rd byte
    * ...                           ...
    */
  uint8_t size = MAX_BYTE_FRAME_SIZE;
  uint8_t frame[size];
  uint8_t checksum = 0x00;
    //                                          v-- Just a little safety net
    for(uint8_t h = 0; (h < image.getHeight()) && (h < 8); h++) {
      uint8_t value = image.getBitmap()[h];
      checksum = (checksum + value) % 256;
      frame[h+1] = value;
    }

    for(uint8_t h = image.getHeight(); h < 8; h++) {
      frame[h+1] = 0x00;
    }

  // building the frame
  frame[0] = PREAMBLE;
  frame[size-1] = checksum;
  buildBitFrame(frame, size, (uint8_t*)_frame, sizeof(_frame));
  // set the transmitter into busy mode so the idle pattern will no longer
  // be transmitted but instead the frame will be sent out.
  _busy = true;
  _pos = 0;

  return 1; //preperation was successful
}

uint8_t Transmitter::buildBitFrame(uint8_t* frame, uint8_t size, uint8_t* outFrame, size_t outFrameSize) {
  _frameSize = size*8;
  if(outFrameSize < _frameSize) {
     // print error: the encoded frame cannot fit in the
     // outFrame buffer of size outFrameSize
     return 0;
  }
  uint8_t pos = 0;
    for (uint8_t byte = 0; byte < size; byte++) {
      for (uint8_t bit = 0; bit < 8; bit++) {
        outFrame[pos] = (frame[byte] >> (7 - bit)) & 0x01;
        pos++;
      }
    }

  return pos;
}


void Transmitter::transmitBit() {
  if(_active) {
    // Preperation
    // only change the state when the manchester bit is completely transmitted.
    if(!_manHalf) {
      if(_busy) {
        // if there is currently a transmission running
        // read the next bit from the frame
        if(_pos < _frameSize) {
          _state = _frame[_pos];
          _pos ++;
        } else  {
          _busy = false;
          _state = 0;
          _pos = 0;
        }
      } else { // else transmit the idle pattern
        _state = 0;
      }
    }
    // Transmission
    if (!_manHalf) {
      digitalWrite(_pin, !_state);
    } else {
      digitalWrite(_pin, _state);
    }
    _manHalf = !_manHalf;
  } else {
    digitalWrite(_pin, HIGH);
  }
}

void Transmitter::start() {
  _instance = this;
  Timer2.attachInterrupt(_transmitBit);
  _active = true;
}

void Transmitter::stop() {
  Timer2.detachInterrupt();
  _active = false;
}

static void Transmitter::_transmitBit() {
  _instance->transmitBit();
}

bool Transmitter::lastTransmissionComplete() {
  return !_busy;
}

void Transmitter::setPin(int pin) {
  _pin = pin;
  pinMode(_pin, OUTPUT);
}

int Transmitter::getPin() {
  return _pin;
}
