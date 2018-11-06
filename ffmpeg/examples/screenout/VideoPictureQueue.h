#ifndef VPQ_H
#define VPQ_H

#include <list>
#include <memory>

#include "library/basic/exmutex.h"

using namespace std;
using namespace vh1981lib;

class VideoPicture {
public:
    VideoPicture();
    virtual ~VideoPicture();
public:
    unsigned char *_yPlane, *_uPlane, *_vPlane;
    size_t _yPlaneSz, _uPlaneSz, _vPlaneSz;
    int _yPitch, _uPitch, _vPitch;

public:
    void init(int yPlaneSz, int uPlaneSz, int vPlaneSz, int yPitch, int uPitch, int vPitch);
    void cleanup();
};

class VideoPictureQueue
{
public:
    VideoPictureQueue();
    virtual ~VideoPictureQueue() {}

    enum {
        VPQ_SIZE = 4,
    };

private:
    VideoPicture _queue[VPQ_SIZE];
    int _rIndex, _wIndex, _size;
    exmutex _mutex;

public:
    int size() { return _size; }
    bool full() { return _size == VPQ_SIZE; }
    void init(int yPlaneSz, int uPlaneSz, int vPlaneSz, int yPitch, int uPitch, int vPitch);
    void cleanup();

    VideoPicture* peek();
    VideoPicture* getEmpty();

    bool pop();
    bool push();
};

#endif
