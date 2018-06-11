#ifndef SIMPLEFIFO_H
#define SIMPLEFIFO_H

#include <Arduino.h>

#define MAX_BUFFER_SIZE  9

class SimpleFIFO
{

  public:
    SimpleFIFO(uint8_t size);
    virtual ~SimpleFIFO() {   };

  public: //methods
    bool push(uint8_t elem);
    uint8_t pop();
    uint8_t peek() { return _data[_tail]; };
    void flush();

    bool isFull() { return _count >= _size; };
    bool isEmpty() { return _count == 0; };
    uint8_t getCount() { return _count; };

  private: //members
    uint8_t _data[MAX_BUFFER_SIZE];
    uint8_t _head;
    uint8_t _tail;
    uint8_t _count;
    uint8_t _size;

};

#endif
