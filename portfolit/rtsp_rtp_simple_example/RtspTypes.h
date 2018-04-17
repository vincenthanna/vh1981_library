#ifndef __RTSP_TYPES_H
#define __RTSP_TYPES_H

#include <iostream>

using namespace std;

enum RTSP_CMD_TYPES
{
    RTSP_OPTIONS,
    RTSP_DESCRIBE,
    RTSP_SETUP,
    RTSP_PLAY,
    RTSP_TEARDOWN,
    RTSP_UNKNOWN,
    RTSP_DIRECTIVES_MAX = RTSP_UNKNOWN
};

struct RtspDirective{
    int cmdType;
    string name;
    RtspDirective(int type, const char* n) : cmdType(type), name(n)
    {

    }
};

extern RtspDirective rtspDirectives[5];

#endif
