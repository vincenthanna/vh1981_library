#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "StreamQueue"
#endif

#include "library/basic/exlog.h"
#include "StreamQueue.h"
#include <stdio.h>
#include <string.h>

StreamQueue::StreamQueue() : _size(DEFAULT_QUEUE_SIZE)
    ,_front(0), _rear(0)
{
    _dataBuf.reset(new unsigned char[_size], [](unsigned char *p) { delete[] p; });
}

StreamQueue::StreamQueue(int size) : _size(size)
    ,_front(0), _rear(0)

{
    _dataBuf.reset(new unsigned char[_size], [](unsigned char *p) { delete[] p; });
}

bool StreamQueue::put(unsigned char* data, int size)
{
    if (freeSize() < size) {
        return false;
    }
    else {
        int remained = size;
        while(remained) {
            int copylen = 0;
            if (_front <= _rear) {
                copylen = (_size - _rear) > remained ? remained : (_size - _rear);
            }
            else {
                copylen = remained;
            }
            memcpy(&_dataBuf.get()[_rear], &data[size - remained], copylen);
            _rear = (_rear + copylen) % _size;
            remained -= copylen;
        }
        return true;
    }
}

bool StreamQueue::get(unsigned char* out, int size)
{
    if (dataSize() < size) {
        return false;
    }
    else {
        int remained = size;
        while (remained) {
            int copylen = 0;
            if (_front <= _rear) {
                copylen = remained;
            }
            else {
                copylen = (_size - _front) > remained ? remained : (_size - _front);
            }
            //EXCLOG(LOG_INFO, "_front=%d _rear=%d start=%d", _front, _rear, _dataBuf.get()[_front]);
            memcpy(&out[size - remained], &_dataBuf.get()[_front], copylen);
            _front = (_front + copylen) % _size;
            remained -= copylen;
        }
        return true;
    }
}

unsigned char* StreamQueue::data()
{
    return &_dataBuf.get()[_front];
}

int StreamQueue::dataSize()
{
    if (_front <= _rear) {
        return _rear - _front;
    }
    else {
        return _size - (_front - _rear);
    }
}

int StreamQueue::freeSize()
{
    return _size - dataSize() - 1;
}

bool StreamQueue::full()
{
    return ((_rear + 1) % _size) == _front;
}

bool StreamQueue::empty()
{
    return _front == _rear;
}


// <EOF>


