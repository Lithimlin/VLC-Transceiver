#include <Reciever.h>

Reciever::Reciever(int frequency) :
  _transmit_period((1.0/frequency)*1000000.0),
  _currentReception(),
  _lastReception(),
  _started(false),
  _bitBuffer(8)
{
  _pin = 0;
  _hadError = false;
  _state = State::RX_IDLE;
  _lastTime = micros();
  clear();
  #ifdef DEBUG
  Serial.println("\n---\nReciever initialized.");
  Serial.print(" Frequency is "); Serial.print(frequency); Serial.println("Hz");
  Serial.print(" Transmit Period is "); Serial.print(_transmit_period); Serial.println("µs");
  Serial.println("---\n");
  #endif
}

Reciever* Reciever::_instance;

void Reciever::start() {
  if(_pin != 0) {
    _instance = this;
    attachInterrupt(digitalPinToInterrupt(_pin), _switchState, CHANGE);
    _started = true;
    #ifdef DEBUG
    Serial.println("Reciever started!");
    #endif
  }
}

static void Reciever::_switchState() {
  _instance->switchState();
}

void Reciever::stop() {
  detachInterrupt(digitalPinToInterrupt(_pin));
  _started = false;
  #ifdef DEBUG
  Serial.println("Reciever stopped!");
  #endif
}

int Reciever::setFrequency(int frequency) {
  if(_started) {
    return 1;
  }
  _transmit_period = (1.0/frequency)*1000000.0; //(1/f)*10^6
  #ifdef DEBUG
  Serial.print("Frequency updated to "); Serial.print(frequency); Serial.println("Hz");
  Serial.print("Transmit Period updated to "); Serial.print(_transmit_period); Serial.println("µs");
  #endif
  return 0;
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
        #ifdef DEBUG
        Serial.print("Measured time: "); Serial.print(t); Serial.println(" (idle)");
        #endif
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
        #ifdef DEBUG
        Serial.print("Measured time: "); Serial.print(t); Serial.println(" (start)");
        #endif
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
        #ifdef DEBUG
        Serial.print("Measured time: "); Serial.print(t); Serial.println(" (alt)");
        #endif
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
        #ifdef DEBUG
        Serial.print("Measured time: "); Serial.print(t); Serial.println(" (rep1)");
        #endif
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
        #ifdef DEBUG
        Serial.print("Measured time: "); Serial.print(t); Serial.println(" (rep2)");
        #endif
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
      #if DEBUG>1
      Serial.println("Fetch preamble");
      #endif
      checkPreamble(); // ...check it.
      break;
    // if the preamble was correct, fetch the data type...
    case ProcessState::FETCH_TYPE:
      #if DEBUG>1
      Serial.println("Fetch type");
      #endif
      readType();
      break;
    // if the type has been fetched, fetch the size next...
    case ProcessState::FETCH_SIZE:
      #if DEBUG>1
      Serial.println("Fetch size");
      #endif
      readSize();
      break;
    // if the size has been fetched, fetch the data...
    case ProcessState::FETCH_DATA:
      #if DEBUG>1
      Serial.println("Fetch data");
      #endif
      // if all data has been recieved...
      if(_recievedByteCtr == _currentReception.size[0]+1) {
        #if DEBUG>1
        Serial.print("Recieved "); Serial.print(_recievedByteCtr);
        Serial.print(" data bytes (of "); Serial.print(_currentReception.size[0]+1); Serial.println(")");
        #endif
        _processState = ProcessState::PROCESS_DATA; // ...go to the next step
      }
      break;
    // if the data is complete...
    case ProcessState::PROCESS_DATA:
      #if DEBUG>1
      Serial.println("Process data");
      #endif
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
          default:
            #ifdef DEBUG
            Serial.print("Invalid data type "); Serial.println(_currentReception.type);
            #endif
            handleError();
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
  #if DEBUG>2
  Serial.print("Pushing value "); Serial.println(value, BIN);
  #endif
  if(!_bitBuffer.push(value)) {
    #ifdef DEBUG
    Serial.println("Buffer full");
    #endif
    uint8_t byte = 0;
    //...empty out _bitBuffer...
    while (!_bitBuffer.isEmpty()) {
      uint8_t ctr = _bitBuffer.getCount();
      byte |= _bitBuffer.pop() << (ctr - 1);
    }
    #ifdef DEBUG
    Serial.print("Read "); Serial.print(byte, HEX); Serial.println(" from buffer");
    #endif
    //...save byte in the _data array...
    _data[_recievedByteCtr] = byte;

    _recievedByteCtr++;
    //...and finally push the value
    _bitBuffer.push(value);
    #if DEBUG>2
    Serial.print("Repushing value "); Serial.println(value);
    #endif
  }
  //increases the _recievedByteCtr if an entire byte has been recieved.
}

void Reciever::checkPreamble() {
  //if the preamble has not been checked yet but an entire byte has been recieved...
  if (_recievedByteCtr == 1) {
    #ifdef DEBUG
    Serial.println("Checking preamble");
    Serial.print("Recieved byte was "); Serial.println(_data[0], HEX);
    #endif
    //...check if it is the PREAMBLE...
    if (_data[0] == PREAMBLE) {
      #ifdef DEBUG
      Serial.println("Check successful");
      #endif
      _processState = ProcessState::FETCH_TYPE;
    } else {
      #ifdef DEBUG
      Serial.println("Check failed");
      #endif
      _processState = ProcessState::ERROR;
    }
    //...and put the counter and data back to 0
    _recievedByteCtr = 0;
    _data[0] = 0;
  }
}

void Reciever::readType() {
  #ifdef DEBUG
  Serial.println("Reading type");
  #endif
  //if there is an entire byte in the buffer...
  if (_recievedByteCtr == 1) {
    //... read it and put it into the transmission...
    _currentReception.type = _data[0]; //1: string, 2: bitmap
    #ifdef DEBUG
    Serial.print("Read type "); Serial.println(_data[0]);
    #endif
    _processState = ProcessState::FETCH_SIZE;
    //... and reset the counter and data back to 0
    _recievedByteCtr = 0;
    _data[0] = 0;
  }
}

void Reciever::readSize() {
  //distinguish between bitmap and string. Btimap has two sizes, string has one.
  switch (_currentReception.type) {
    case 1: //string
      //if the single size byte has been received...
      if(_recievedByteCtr == 1) {
        //... read it to the first position of _dataSize...
        _currentReception.size[0] = _data[0];
        _currentReception.size[1] = 0; //... and clear the second position just to be safe
        #ifdef DEBUG
        Serial.print("String size: "); Serial.println(_data[0]);
        #endif
        //also reset the counter and data back to 0
        _recievedByteCtr = 0;
        _data[0] = 0;
        _processState = ProcessState::FETCH_DATA;
      }
      break;
    case 2: //bitmap
      //if the two size bytes have been received...
      if(_recievedByteCtr == 2) {
        //... read them into _dataSize...
        _currentReception.size[0] = _data[0];
        _currentReception.size[1] = _data[1];
        #ifdef DEBUG
        Serial.print("Image size: "); Serial.print(_data[0]);
        Serial.print("\tImage height: "); Serial.println(_data[1]);
        #endif
        //... and reset the counter and data back to 0
        _recievedByteCtr = 0;
        _data[0] = 0;
        _data[1] = 0;
        _processState = ProcessState::FETCH_DATA;
      }
  }
}

bool Reciever::checksumCorrect() {
  #ifdef DEBUG
  Serial.println("Checking checksum");
  #endif
  uint8_t recievedChecksum = 0;
  for (uint8_t i = 0; i < _currentReception.size[0]; i++) {
    recievedChecksum += _data[i];
    recievedChecksum %= 256;
  }
  #ifdef DEBUG
  Serial.print("Own Checksum: "); Serial.print(recievedChecksum, HEX);
  Serial.print("\tSent Checksum: "); Serial.println(_data[_currentReception.size[0]], HEX);
  #endif
  return (recievedChecksum == _data[_currentReception.size[0]]);
}

//TODO!!!! ----------------------------------------- !!!!! --------------------------------
void Reciever::buildImage() {
  _lastReception.type = 2;
  uint8_t height = _currentReception.size[0];
  uint8_t size = _currentReception.size[1];
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
  _lastReception.data.string = _data; // This SHOULD work
  #ifdef DEBUG
  Serial.print("Built string: \""); Serial.print(_lastReception.data.string); Serial.println('"');
  #endif
}

void Reciever::handleError() {
  #ifdef DEBUG
  Serial.println("Error detected");
  #endif
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

String Reciever::getString() {
  if(_lastReception.type==1)
    return _lastReception.data.string;
  else
    return String("");
}

int Reciever::getType() {
  return _lastReception.type;
}
