#include "Packet.h"

using namespace std;
using namespace vh1981lib;

Packet::Packet() : _buffer(nullptr),
    _bufSize(2048),
    _dataLen(0)
{
    init();
}

Packet::Packet(size_t bufSize) :
    _bufSize(bufSize),
    _dataLen(0)
{
    init();
}

Packet::~Packet()
{
    if (_buffer) {
        delete []_buffer;
    }
}

void Packet::init()
{
    _buffer = new unsigned char[_bufSize];
}

void Packet::clear()
{
    _dataLen = 0;
}
