#ifndef STREAMQUEUE_H
#define STREAMQUEUE_H

#include <list>
#include <memory>

#include "library/basic/exmutex.h"

using namespace vh1981lib;

using namespace std;

class StreamQueue
{
public:
    StreamQueue();
    StreamQueue(int size);
    virtual ~StreamQueue() {}

public:
    enum {
        DEFAULT_QUEUE_SIZE = 1024,
    };

private:
    int _size;
    shared_ptr<unsigned char> _dataBuf;
    unsigned int _front, _rear;

public:
    shared_ptr<unsigned char>& dataBuf();
    int size() { return _size; }
    int freeSize();
    int dataSize();
    bool full();
    bool empty();

    bool put(unsigned char* data, int size);
    bool get(unsigned char* out, int size);
    unsigned char* data();
};

#endif // STREAMQUEUE_H


// <EOF>

