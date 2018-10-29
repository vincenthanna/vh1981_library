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

#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>

#include "StreamQueue.h"

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
    AVPacket _pkt;
    AVFrame* _frame;

    int _audio_stream_idx;
    AVCodecContext* _audio_dec_ctx;
    AVStream* _audio_stream;

    list<PlayerListener*> _playerListenerList;

    struct SwsContext* _sws_ctx;
    unsigned char *_yPlane, *_uPlane, *_vPlane;
    size_t _yPlaneSz, _uvPlaneSz;
    int _uvPitch;

    AVPacket _audioPkt;
    uint8_t* _audio_pkt_data;
    int _audio_pkt_size;
    AVFrame* _aFrame;

    SwrContext *_audio_swr;

    uint8_t** _audio_outputBuffer;
    int _audio_dst_linesize;

    enum {
        MAX_AUDIO_FRAME_SIZE = 16384,
    };

    unsigned char _audio_buf[(MAX_AUDIO_FRAME_SIZE * 3) / 2];
    unsigned int _audio_buf_size;
    unsigned int _audio_buf_index;


public:
    /**
     @name decoding
    */
    //@{
private:
    int decode_packet(int *got_frame, int cached);
    int open_codec_context(int *stream_idx, AVFormatContext *fmt_ctx, enum AVMediaType type);
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

    void audioCallback (unsigned char* stream, int len);
    int audio_decode_frame(uint8_t *audio_buf, int buf_size);

#if 0
    class PacketQueue {
    public:
        AVPacketList *_first_pkt, *_last_pkt;
        int _nb_packets;
        int _size;
        SDL_mutex *_mutex;
        SDL_cond *_cond;
        bool _quit;



    public:
        PacketQueue();
        virtual ~PacketQueue() {}

        void init();
        int put(AVPacket *pkt);
        int get(AVPacket *pkt, int block);
    };

    PacketQueue _audioPacketQueue;
#endif

    StreamQueue _audioStreamQueue;

};

void audio_callback (void *userdata, unsigned char* stream, int len);

#endif // PLAYER_H
