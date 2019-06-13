#include "RtspTypes.h"

RtspDirective rtspDirectives[5] = {
    RtspDirective(RTSP_OPTIONS, "OPTIONS"),
    RtspDirective(RTSP_DESCRIBE, "DESCRIBE"),
    RtspDirective(RTSP_SETUP, "SETUP"),
    RtspDirective(RTSP_PLAY, "PLAY"),
    RtspDirective(RTSP_TEARDOWN, "TEARDOWN"),
};
