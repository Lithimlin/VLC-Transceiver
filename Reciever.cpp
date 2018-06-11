#include <Reciever.h>

Reciever::Reciever() :
  _bitBuffer(8),
  _image(IMAGE_WIDTH, IMAGE_HEIGHT)
{
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
  //Serial.println(t);
  _lastTime = micros();
  switch (_state) {
    //Idle
    case State::RX_IDLE:
      if (IN_T1(t)) {
        clear();
        //Serial.println("Continue Idle");
      } else if(IN_T2(t)) {
        _state = State::RX_START_RECEPTION;
        _value = digitalRead(_pin);
        pushValue(_value);
        //Serial.println("Idle to Start");
        //Serial.print("Value is ");
        //Serial.println(_value);
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
        //Serial.println("Start to Idle");
      //if recorded period is 2T
      } else if(IN_T2(t)) {
        _state = State::RX_BIT_ALTERATION;
        _value = !_value;
        pushValue(_value);
        _recieving = true;
        //Serial.println("Start to Alt");
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
            //Serial.println("Continue Alt");
          }
        }
      //if recorded period is T
      } else if(IN_T1(t)) {
        _state = State::RX_BIT_REPITITION1;
        //Serial.println("Alt to Rep1");
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
            //Serial.println("Rep1 to Rep2");
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
        //Serial.println("Rep2 to Rep1");
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
            //Serial.println("Rep2 to Alt");
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
    // if the preamble was correct, fetch the data...
    case ProcessState::FETCH_DATA:
      // if all data has been recieved...
      if(_recievedByteCtr == IMAGE_HEIGHT+1)
        _processState = ProcessState::DATA_COMPLETE; // ...go to the next step
      break;
    // if the data is complete...
    case ProcessState::DATA_COMPLETE:
      // ...check the checksum...
      if(checksumCorrect()) {
        _success = true;
        buildImage(); //...and build the image if it was correct...
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
  //Serial.println("In pushValue");
  // Serial.print("Pushed Value ");
  // Serial.println(value);
  // if the _bitBuffer is full (holds an entire byte)...
  if(!_bitBuffer.push(value)) {
    uint8_t byte = 0;
    //...empty out _bitBuffer...
    while (!_bitBuffer.isEmpty()) {
      // Serial.println("_bitbuffer full");
      uint8_t ctr = _bitBuffer.getCount();
      // Serial.print(ctr);
      // Serial.println(" bits left in buffer");
      // Serial.print("Pushing ");
      // Serial.println(_bitBuffer.peek(), BIN);
      byte |= _bitBuffer.pop() << (ctr - 1);
      // Serial.print("Pushed to ");
      // Serial.println(byte, BIN);
    }
    //...save byte in the _data array...
    _data[_recievedByteCtr] = byte;
    // Serial.print("Recieved Data:");
    // Serial.println(_data[_recievedByteCtr], BIN);

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
      _processState = ProcessState::FETCH_DATA;
    } else {
      _processState = ProcessState::ERROR;
    }
    //...and put the counter and data back to 0
    _recievedByteCtr = 0;
    _data[0] = 0;
  }
}

bool Reciever::checksumCorrect() {
  uint8_t recievedChecksum = 0;
  for (uint8_t i = 0; i < IMAGE_HEIGHT; i++) {
    recievedChecksum += _data[i];
    recievedChecksum %= 256;
  }
  return (recievedChecksum == _data[IMAGE_HEIGHT]);

}

void Reciever::buildImage() {
  uint8_t map[IMAGE_HEIGHT];
  for(uint8_t a = 0; a < IMAGE_HEIGHT; a++) {
    map[a] = _data[a];
  }
  _image.buildFromBytemap(map);
}

LEDBitmap Reciever::getImage() {
  return _image;
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
