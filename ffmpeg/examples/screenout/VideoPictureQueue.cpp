#include "VideoPictureQueue.h"

VideoPicture::VideoPicture() : _yPlane(nullptr), _uPlane(nullptr), _vPlane(nullptr)
    ,_yPlaneSz(0), _uPlaneSz(0), _vPlaneSz(0)
    ,_yPitch(0), _uPitch(0), _vPitch(0)
{
}

VideoPicture::~VideoPicture()
{
    cleanup();
}

void VideoPicture::init(int yPlaneSz, int uPlaneSz, int vPlaneSz, int yPitch, int uPitch, int vPitch)
{
    _yPlaneSz = yPlaneSz;
    _uPlaneSz = uPlaneSz;
    _vPlaneSz = vPlaneSz;

    _yPitch = yPitch;
    _uPitch = uPitch;
    _vPitch = vPitch;

    _yPlane = (unsigned char*)malloc(_yPlaneSz);
    _uPlane = (unsigned char*)malloc(_uPlaneSz);
    _vPlane = (unsigned char*)malloc(_vPlaneSz);
}

void VideoPicture::cleanup()
{
    if (_yPlane) {
        free(_yPlane);
        _yPlane = nullptr;
    }
    if (_uPlane) {
        free(_uPlane);
        _uPlane = nullptr;
    }
    if (_vPlane) {
        free(_vPlane);
        _vPlane = nullptr;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////

VideoPictureQueue::VideoPictureQueue() : _rIndex(0), _wIndex(0), _size(0)
{
}

void VideoPictureQueue::init(int yPlaneSz, int uPlaneSz, int vPlaneSz, int yPitch, int uPitch, int vPitch)
{
    for (int i = 0; i < VPQ_SIZE; i++) {
        _queue[i].init(yPlaneSz, uPlaneSz, vPlaneSz, yPitch, uPitch, vPitch);
    }
}

void VideoPictureQueue::cleanup()
{
    for (int i = 0; i < VPQ_SIZE; i++) {
        _queue[i].cleanup();
    }
}

VideoPicture* VideoPictureQueue::peek()
{
    autoexmutex a(_mutex);
    if (_size == 0) {
        return nullptr;
    }
    else {
        return &_queue[_rIndex];
    }
}

VideoPicture* VideoPictureQueue::getEmpty()
{
    autoexmutex a(_mutex);
    if (_size == VPQ_SIZE) {
        return nullptr;
    }
    else {
        return &_queue[_wIndex];
    }
}

bool VideoPictureQueue::pop()
{
    autoexmutex a(_mutex);
    if (_size == 0) {
        return false;
    }
    else {
        _rIndex = (_rIndex + 1) % VPQ_SIZE;
        _size--;
    }
}

bool VideoPictureQueue::push()
{
    autoexmutex a(_mutex);
    if (_size == VPQ_SIZE) {
        return false;
    }
    else {
        _wIndex = (_wIndex + 1) % VPQ_SIZE;
        _size++;
    }
}


