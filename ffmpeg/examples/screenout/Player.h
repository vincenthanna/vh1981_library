#ifndef PLAYER_H
#define PLAYER_H


extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswresample/swresample.h>
    #include <libswscale/swscale.h>

    #include <libavformat/avformat.h>
    #include <libavformat/avio.h>
    #include <libavutil/opt.h>
    #include <libavutil/file.h>
    #include <libavutil/timestamp.h>

    #include <libavutil/imgutils.h>
    #include <libavutil/samplefmt.h>

};

#include "PlayerListener.h"
#include <list>
#include <memory>

using namespace std;

class Player
{
public:
    Player();

private:
    AVFormatContext* _fmt_ctx;
    int _video_stream_idx;
    AVCodecContext* _video_dec_ctx;
    AVStream* _video_stream;
    AVPacket pkt;
    AVFrame *frame;
    list<PlayerListener*> _playerListenerList;

    struct SwsContext* sws_ctx;
    unsigned char *_yPlane, *_uPlane, *_vPlane;
    size_t _yPlaneSz, _uvPlaneSz;
    int _uvPitch;


public:
    /**
     @name decoding
    */
    //@{
private:
    int decode_packet(int *got_frame, int cached);
    int open_codec_context(int *stream_idx, AVFormatContext *fmt_ctx, enum AVMediaType type, \
                           const char* src_filename);
    //@}

    /**
     @name AVFrame을 jpeg 파일로 출력하는 함수
     */
    //@{
private:
    bool writeJPEG (const AVFrame *pFrame);
    //@}

    /**
     @name 외부 인터페이스
     */
    //@{
public:
    void play(const char* filename);
    void addPlayerListener(const PlayerListener* listener);
    //@}

public:


};

#endif // PLAYER_H
