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

Transmitter::Transmitter(int frequency)
{
  _pin = 0;
  _pos = 0;
  _manHalf = 0;
  _busy = false;
  _state = 0;
  _active = false;
  Timer2.initialize(frequency);
}

Transmitter* Transmitter::_instance;

/**
  * sends a LEDBitmap of a width of 8 and a height up to 8
  * via the specified LED pin.
  *
  * @param image  The image that is to be transmitted.
  *
  * @return 0 if the transmission was successful,
  *         1 if there is already a transmission active,
  *         2 if no transmission pin has been specified.
  */
int Transmitter::sendData(LEDBitmap &image) {
  return _prepFrame(image);
}

/**
  * sends a string of a size up to 255 characters
  * via the specified LED pin.
  *
  * @param image  The string that is to be transmitted.
  *
  * @return 0 if the transmission was successful,
  *         1 if there is already a transmission active,
  *         2 if no transmission pin has been specified.
  */
int Transmitter::sendData(String &string) {
  return _prepFrame(string)
}

/**
  * prepares a LEDBitmap for transmission on the specified LED pin.
  *
  * @param image  The image that is to be prepared.
  *
  * @return 0 if the transmission/preparation was successful,
  *         1 if there is already a transmission active,
  *         2 if no transmission pin has been specified.
  */
int Transmitter::_prepFrame(LEDBitmap &image) {
  if(_busy) return 1; //no Data can be sent right now since the transmitter is already busy
  if(!_pin) return 2;
  /** construct the intermediate byte frame.
    *
    * frame structure is
    * 1 octet preamble (0xA5) | 1 octet type | 2 octets size* | up to 255* octets payload | 1 octet checksum
    *
    * the size here consists of the total size and the height of the bitmap
    * up to 8 octets in the case of a bitmap
    *
    * the image will be transmitted in rows
    * with one row being one byte/octet
    *  1  2  3  4  5  6  7  8   <-- 1st octet
    *  9 10 11 12 13 14 15 16   <-- 2nd octet
    * 17 18 19 20 21 22 23 24   <-- 3rd octet
    * ...                           ...
    */
  uint8_t img_size = image.getSize()
  uint8_t size = img_size + 5;
  uint8_t *frame = (uint8_t*)malloc(size*sizeof(uint8_t));
  uint8_t checksum = 0x00;
  //                                          v-- Just a little safety net
  for(uint8_t h = 0; (h < image.getHeight()) && (h < 8); h++) {
    uint8_t value = image.getBitmap()[h];
    checksum = (checksum + value) % 256;
    frame[h+4] = value;
  }

  /* Fill up the rest of the image with 0 values. Legacy code.
  for(uint8_t h = image.getHeight(); h < 8; h++) {
    frame[h+1] = 0x00;
  }
  */

  // building the frame
  frame[0] = PREAMBLE;
  frame[1] = 2; // type = 2 = bitmap
  frame[2] = img_size;
  frame[3] = image.getHeight();
  frame[size-1] = checksum;
  _buildBitFrame(frame, size);
  // set the transmitter into busy mode so the idle pattern will no longer
  // be transmitted but instead the frame will be sent out.
  free(frame);
  _busy = true;
  _pos = 0;

  return 0; //preperation was successful
}

/**
  * prepares a string for transmission
  *
  * @param  str The string that is to be prepared
  *
  * @return 0 if the transmission/preparation was successful,
  *         1 if there is already a transmission active,
  *         2 if no transmission pin has been specified.
  */
int Transmitter::_prepFrame(String &string){
  if(_busy) return 1; //no Data can be sent right now since the transmitter is already busy
  if(!_pin) return 2;
  /** construct the intermediate byte frame.
    *
    * frame structure is
    * 1 octet preamble (0xA5) | 1 octet type | 1 octet size | up to 255* octets payload | 1 octet checksum
    *
    * up to 255 octets in the case of a string. One octet for each char in the string
    */
  uint8_t str_size = string.length() + 1; //One more to also transmit the end character!
  uint8_t size = str_size + 4;
  uint8_t* frame = (uint8_t*)malloc(size*sizeof(uint8_t))
  uint8_t checksum = 0x00;

  for(uint8_t c = 0; c < str_size; c++){
    uint8_t value = string[c];
    checksum = (checksum + value) % 256;
    frame[c+3] = value;
  }

  frame[0] = PREAMBLE;
  frame[1] = 1; //type = 1 = string
  frame[2] = str_size;
  frame[size-1] = checksum;
  _buildBitFrame(frame, size);
  // set the transmitter into busy mode so the idle pattern will no longer
  // be transmitted but instead the frame will be sent out.
  free(frame);
  _busy = true;
  _pos = 0;

  return 0; //preperation was successful
}

/**
  * Builds a bit frame from an intermediate byte frame.
  *
  * @param frame  A pointer to the first element of the intermediate byte frame
  *
  * @param size   The size of the intermediate byte frame
  *
  * @return the position after the final position (and thus size) of the bit frame
  */
uint8_t Transmitter::_buildBitFrame(uint8_t* frame, uint8_t &size) {
  /** constructs the bitwise frame for transmission. Takes the intermediate
    * frame consisting of bytes and puts every bit into its own byte so it can
    * be transmitted.
    * Ex: 10100101 -> 0000001 00000000 00000001 00000000 00000000 00000001 00000000 000000001
    */
  _frame.size = size*8;
  _frame.data = (uint8_t*)malloc(_frame.size*sizeof(uint8_t))
  uint8_t pos = 0;
    for (uint8_t byte = 0; byte < size; byte++) {
      for (uint8_t bit = 0; bit < 8; bit++) {
        _frame.data[pos] = (frame[byte] >> (7 - bit)) & 0x01;
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
        if(_pos < _frame.size) {
          _state = _frame.data[_pos];
          _pos ++;
        } else  {
          _busy = false;
          _state = 0;
          _pos = 0;
          free(_frame.data);
          _frame.size = 0;
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

int Transmitter::setFrequency(int frequency) {
  if(_active) {
    return 1;
  }
  Timer2.initialize(frequency);
  return 0;
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
