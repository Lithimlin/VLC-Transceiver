#include <Reciever.h>

Reciever::Reciever() :
  _bitBuffer(8)
{
  _currentReception.type=0;
  _lastReception.type=0;
  _started = false;
  _pin = 0;
  _hadError = false;
  _state = State::RX_IDLE;
  _lastTime = micros();
  clear();
}

Reciever* Reciever::_instance;

void Reciever::start() {
  if(_pin != 0) {
    //Serial.println("Reciever started!");
    _instance = this;
    attachInterrupt(digitalPinToInterrupt(_pin), _switchState, CHANGE);
    _started = true;
  }
}

static void Reciever::_switchState() {
  _instance->switchState();
}

void Reciever::stop() {
  detachInterrupt(digitalPinToInterrupt(_pin));
  _started = false;
}

bool Reciever::hadError() {
  bool tmp = _hadError;
  _hadError = false;
  return tmp;
}

bool Reciever::receptionSuccessful() {
  bool tmp = _success;
  _success = false;
  return tmp;
}

void Reciever::switchState() {
  unsigned long t = micros() - _lastTime;
  _lastTime = micros();
  switch (_state) {
    //Idle
    case State::RX_IDLE:
      if (IN_T1(t)) {
        clear();
      } else if(IN_T2(t)) {
        _state = State::RX_START_RECEPTION;
        _value = digitalRead(_pin);
        pushValue(_value);
      } else {
        handleError();
      }
      break;

    //Start
    case State::RX_START_RECEPTION:
      //if recorded period is T
      if(IN_T1(t)) {
        handleError();
        _state = State::RX_IDLE;
      //if recorded period is 2T
      } else if(IN_T2(t)) {
        _state = State::RX_BIT_ALTERATION;
        _value = !_value;
        pushValue(_value);
        _recieving = true;
      } else {
        handleError();
        _state = State::RX_IDLE;
      }
      break;

    //Bit Alteration
    case State::RX_BIT_ALTERATION:
      //if recorded period is 2T
      if(IN_T2(t)) {
        _value = !_value;
        process(_value);
        if(_processState == ProcessState::RECEPTION_FINISHED) {
          _state = State::RX_IDLE;
          _recieving = false;
        } else {
          if(_processState == ProcessState::ERROR) {
            handleError();
            _state = State::RX_IDLE;
          } else {
            _state = State::RX_BIT_ALTERATION;
          }
        }
      //if recorded period is T
      } else if(IN_T1(t)) {
        _state = State::RX_BIT_REPITITION1;
      } else {
        handleError();
        _state = State::RX_IDLE;
      }
      break;

    //Bit Repitition (1st State)
    case State::RX_BIT_REPITITION1:
      //if recorded period is T
      if(IN_T1(t)) {
        process(_value);
        if(_processState == ProcessState::RECEPTION_FINISHED) {
          _state = State::RX_IDLE;
          _recieving = false;
        } else {
          if(_processState == ProcessState::ERROR) {
            handleError();
            _state = State::RX_IDLE;
          } else {
            _state = State::RX_BIT_REPITITION2;
          }
        }
      //if recorded period is 2T or anything else
      } else {
        handleError();
        _state = State::RX_IDLE;
      }
      break;

    //Bit Repitition (2nd State)
    case State::RX_BIT_REPITITION2:
      //if recorded period is T
      if(IN_T1(t)) {
        _state = State::RX_BIT_REPITITION1;
      //if recorded period is 2T
      } else if(IN_T2(t)) {
        _value = !_value;
        process(_value);
        if(_processState == ProcessState::RECEPTION_FINISHED) {
          _state = State::RX_IDLE;
          _recieving = false;
        } else {
          if(_processState == ProcessState::ERROR) {
            handleError();
            _state = State::RX_IDLE;
          } else {
            _state = State::RX_BIT_ALTERATION;
          }
        }
      //if recorded period is 2T or anything else
      } else {
        handleError();
        _state = State::RX_IDLE;
      }
  }
}

// -------------------------------------------------------------------------- //

void Reciever::process(uint8_t value) {
  //Serial.println("In process...");
  pushValue(value);
  switch (_processState) {
    // if the preamble has not been checked yet...
    case ProcessState::FETCH_PREAMBLE:
      checkPreamble(); // ...check it.
      break;
    // if the preamble was correct, fetch the data type...
    case ProcessState::FETCH_TYPE:
      readType();
      break;
    // if the type has been fetched, fetch the size next...
    case ProcessState::FETCH_SIZE:
      readSize();
      break;
    // if the size has been fetched, fetch the data...
    case ProcessState::FETCH_DATA:
      // if all data has been recieved...
      if(_recievedByteCtr == _dataSize[0]+1)
        _processState = ProcessState::PROCESS_DATA; // ...go to the next step
      break;
    // if the data is complete...
    case ProcessState::PROCESS_DATA:
      // ...check the checksum...
      if(checksumCorrect()) {
        _success = true;
        //... and if it was correct, build whatever we received.
        switch (_currentReception.type) {
          case 1: //string
            buildString();
            break;
          case 2: //bitmap
            buildImage();
            break;
        }
        _processState = ProcessState::RECEPTION_FINISHED;
      } else {
        _processState = ProcessState::ERROR; // ...or signal an error.
      }
      break;
    // this is the error state. It should not be necessary, but just in case...
    case ProcessState::ERROR:
      handleError();
      break;

  }
  // sets the _processState to RECEPTION_FINISHED if
  // the reception is completed and was successful
}

void Reciever::pushValue(uint8_t value) {
  // if the _bitBuffer is full (holds an entire byte)...
  if(!_bitBuffer.push(value)) {
    uint8_t byte = 0;
    //...empty out _bitBuffer...
    while (!_bitBuffer.isEmpty()) {
      uint8_t ctr = _bitBuffer.getCount();
      byte |= _bitBuffer.pop() << (ctr - 1);
    }
    //...save byte in the _data array...
    _data[_recievedByteCtr] = byte;

    _recievedByteCtr++;
    //...and finally push the value
    _bitBuffer.push(value);
  }
  //increases the _recievedByteCtr if an entire byte has been recieved.
}

void Reciever::checkPreamble() {
  //if the preamble has not been checked yet but an entire byte has been recieved...
  if (_recievedByteCtr == 1) {
    //...check if it is the PREAMBLE...
    if (_data[0] == PREAMBLE) {
      _processState = ProcessState::FETCH_TYPE;
    } else {
      _processState = ProcessState::ERROR;
    }
    //...and put the counter and data back to 0
    _recievedByteCtr = 0;
    _data[0] = 0;
  }
}

void Reciever::readType() {
  //if there is an entire byte in the buffer...
  if (_recievedByteCtr == 1) {
    //... read it and put it into the transmission...
    _currentReception.type = _data[0]; //1: string, 2: bitmap
    _processState = ProcessState::FETCH_SIZE;
    //... and reset the counter and data back to 0
    _recievedByteCtr = 0;
    _data[0] = 0;
  }
}

void Receiver::readSize() {
  //distinguish between bitmap and string. Btimap has two sizes, string has one.
  switch (_currentReception.type) {
    case 1: //string
      //if the single size byte has been received...
      if(_recievedByteCtr == 1) {
        //... read it to the first position of _dataSize...
        _currentReception.size[0] = _data[0];
        _currentReception.size[1] = 0; //... and clear the second position just to be safe
        //also reset the counter and data back to 0
        _recievedByteCtr = 0;
        _data[0] = 0;
      }
      break;
    case 2: //bitmap
      //if the two size bytes have been received...
      if(_recievedByteCtr == 2) {
        //... read them into _dataSize...
        _currentReception.size[0] = _data[0];
        _currentReception.size[1] = _data[1];
        //... and reset the counter and data back to 0
        _recievedByteCtr = 0;
        _data[0] = 0;
        _data[1] = 0;
      }
  }
}

bool Reciever::checksumCorrect() {
  uint8_t recievedChecksum = 0;
  for (uint8_t i = 0; i < _currentReception.size[0]; i++) {
    recievedChecksum += _data[i];
    recievedChecksum %= 256;
  }
  return (recievedChecksum == _data[_currentReception.size[0]]);

}

//TODO!!!! ----------------------------------------- !!!!! --------------------------------
void Reciever::buildImage() {
  _lastReception.type = 2;
  uint8_t size = _currentReception.size[0];
  uint8_t height = _currentReception.size[1];
  uint8_t width = size / height;
  _lastReception.data.bitmap = LEDBitmap(width, height);

  uint8_t map[size];
  for(uint8_t a = 0; a < size; a++) {
    map[a] = _data[a];
  }

  _lastReception.data.bitmap.buildFromBytemap(map);
}

void Reciever::buildString() {
  _lastReception.type = 1;
  _lastReception.data.string = String(_data); // This SHOULD work
}

void Reciever::handleError() {
  _hadError = true;
  clear();
}

void Reciever::clear() {
  _processState = ProcessState::FETCH_PREAMBLE;
  _success = false;
  _bitBuffer.flush();
  _recievedByteCtr = 0;
  _recieving = false;
}

LEDBitmap Reciever::getImage() {
  if(_lastReception.type==2)
    return _lastReception.data.bitmap;
  else
    return LEDBitmap(0, 0);
}

String Receiver::getString() {
  if(_lastReception.type==1)
    return _lastReception.data.String;
  else
    return String("");
}

int Receiver::getType() {
  return _lastReception.type;
}
