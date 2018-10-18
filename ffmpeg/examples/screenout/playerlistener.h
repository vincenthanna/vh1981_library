#ifndef PLAYERLISTENER_H
#define PLAYERLISTENER_H

#include <libavutil/frame.h>

class PlayerListener
{
public:
    virtual void FrameReady(AVFrame *frame) = 0;
};

#endif // PLAYERLISTENER_H
