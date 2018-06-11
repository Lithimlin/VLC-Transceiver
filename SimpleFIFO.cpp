#include <SimpleFIFO.h>

SimpleFIFO::SimpleFIFO(uint8_t size) {
  _size = size;
  flush();
}

bool SimpleFIFO::push(uint8_t elem) {
  if (isFull())
    return false;
  _data[_head] = elem;
  _count++;
  _head++;
  _head %= _size;
  return true;
}

uint8_t SimpleFIFO::pop() {
  if(isEmpty())
    return 2;
  _count--;
  uint8_t elem = _data[_tail] & 0x0001;
  _tail++;
  _tail %= _size;
  return elem;
}

void SimpleFIFO::flush() {
  _head = 0;
  _tail = 0;
  _count = 0;
}
