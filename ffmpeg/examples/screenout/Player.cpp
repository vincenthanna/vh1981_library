#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "Player"
#endif


#include "Player.h"

#include "library/basic/extools.h"
#include "library/basic/exlog.h"
#include "library/basic/exthread.h"
#include <iostream>


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

extern "C"
{
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
}

#include "library/basic/exlog.h"

#include "SDLDisplay.h"

#define SDL_AUDIO_BUFFER_SIZE 2048
#define AUDIO_SAMPLES 2048
#define MAX_AUDIO_FRAME_SIZE 192000

using namespace vh1981lib;

Player::Player() : _playerListenerList()
    , _fmt_ctx(nullptr)
    , _audio_dec_ctx(nullptr)
    ,_sws_ctx(nullptr)
//    ,_audioPacketQueue()
    ,_audio_swr(nullptr)
    ,_audio_buf_size(0)
    ,_audio_buf_index(0)
    ,_audio_pkt_data(nullptr)
    ,_audio_pkt_size(0)
    ,_audioStreamQueue(1024 * 128)
    ,_needAudioConverting(false)
{

}

Player::~Player()
{
    if (_audio_swr) {
        swr_free(&_audio_swr);
    }
}

int Player::decode_packet(int *got_frame)
{
    int ret = 0;
    int decoded = _pkt.size;
    if (_pkt.stream_index == _video_stream_idx) {

        /* decode video frame */
        ret = avcodec_decode_video2(_video_dec_ctx, _frame, got_frame, &_pkt);
        if (ret < 0) {
            EXCLOG(LOG_ERROR, "Error decoding video frame");
            return ret;
        }

        if (*got_frame) {
            //EXCLOG(LOG_INFO, "video_frame%s coded_n:%d size:%d format:%d (%d X %d)", \
                   cached ? "(cached)" : "", _frame->coded_picture_number, _frame->pkt_size, \
                   _frame->format, _frame->width, _frame->height);

            // FIXME: todo

            AVPicture pict;
            pict.data[0] = _yPlane;
            pict.data[1] = _uPlane;
            pict.data[2] = _vPlane;
            pict.linesize[0] = _video_dec_ctx->width;
            pict.linesize[1] = _uvPitch;
            pict.linesize[2] = _uvPitch;

            // Convert the image into YUV format that SDL uses
            sws_scale(_sws_ctx, (uint8_t const * const *) _frame->data,
                    _frame->linesize, 0, _video_dec_ctx->height, pict.data,
                    pict.linesize);

            SDLDisplay::get()->updateTexture(_yPlane, _video_dec_ctx->width, _uPlane, _uvPitch, _vPlane, _uvPitch);

            for (auto listener : _playerListenerList) {
                listener->FrameReady(_frame);
            }
            //EXCLOG(LOG_INFO, "TRACE");
        }
    }
    else if (_pkt.stream_index == _audio_stream_idx) {

        int gotFrame = 0;
        int len = avcodec_decode_audio4(_audio_dec_ctx, _aFrame, &gotFrame, &_pkt);
        if (len >= 0) {

            if (got_frame) {

                if (_needAudioConverting) {
                    //#define AUDIO_WRITE_TO_FILE
#ifdef AUDIO_WRITE_TO_FILE
                    static int audio_frame_count = 0;
                    static FILE* audio_dst_file = nullptr;
                    if (audio_dst_file == nullptr) {
                        audio_dst_file = fopen("audioout.pcm", "a+");
                    }
#endif

                    int samples = swr_convert(_audio_swr, _audio_outputBuffer, _aFrame->nb_samples, \
                            (const uint8_t **)_aFrame->extended_data, _aFrame->nb_samples);

                    int dst_bufsize = av_samples_get_buffer_size(&_audio_dst_linesize, _audio_dec_ctx->channels, samples, AV_SAMPLE_FMT_S16, 1);
#ifdef AUDIO_WRITE_TO_FILE
                    fwrite(_audio_outputBuffer[0], 1, dst_bufsize, audio_dst_file);
                    fclose(audio_dst_file);
                    audio_dst_file = nullptr;
#endif

                    while (!_audioStreamQueue.put(_audio_outputBuffer[0], dst_bufsize)) {
                        exthread::sleep(10);
                    }
                }
                else {
                    int dataSize = av_samples_get_buffer_size(NULL, _audio_dec_ctx->channels, _aFrame->nb_samples, \
                            _audio_dec_ctx->sample_fmt, 1);
                    while (!_audioStreamQueue.put(_aFrame->data[0], dataSize)) {
                        exthread::sleep(10);
                    }
                }
            }
        }
    }
    return decoded;
}

int Player::open_codec_context(int *stream_idx, AVFormatContext *fmt_ctx, enum AVMediaType type)
{
    int ret;
    AVStream *st;
    AVCodecContext *dec_ctx = nullptr;
    AVCodec *dec = nullptr;
    ret = av_find_best_stream(fmt_ctx, type, -1, -1, nullptr, 0);
    if (ret < 0) {
        EXCLOG(LOG_ERROR, "Could not find %s stream", av_get_media_type_string(type));
        return ret;
    }
    else {
        *stream_idx = ret;
        st = fmt_ctx->streams[*stream_idx];
        // find decoder for the stream
        dec_ctx = st->codec;
        dec = avcodec_find_decoder(dec_ctx->codec_id);
        if (!dec) {
            EXCLOG(LOG_ERROR, "Failed to find %s codec", av_get_media_type_string(type));
            return ret;
        }
        if ((ret = avcodec_open2(dec_ctx, dec, nullptr)) < 0) {
            EXCLOG(LOG_ERROR, "Failed to open %s codec", av_get_media_type_string(type));
            return ret;
        }
    }
    return 0;
}

bool Player::writeJPEG(const AVFrame *pFrame)
{
    int got_output = 0;
    int ret;
    AVPacket avPacket;
    static int index=0;

    av_init_packet(&avPacket);
    avPacket.data = nullptr;
    avPacket.size = 0;

    AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
    if (!codec) {
        EXCLOG(LOG_ERROR, "Codec not found\n");
        return false;
    }

    AVCodecContext* c = avcodec_alloc_context3(codec);
    if (!c) {
        EXCLOG(LOG_ERROR, "Could not allocate video codec context\n");
        return false;
    }

    c->bit_rate = 500000;
    c->width = pFrame->width;
    c->height = pFrame->height;
    c->time_base= (AVRational){1,25};
    //c->pix_fmt = AV_PIX_FMT_YUVJ420P;
    c->pix_fmt = static_cast<AVPixelFormat>(pFrame->format);
    cout << "pix_fmt : " << c->pix_fmt << endl;

    if (avcodec_open2(c, codec, nullptr) < 0) {
        EXCLOG(LOG_ERROR, "Could not open codec");
        return false;
    }

    ret = avcodec_encode_video2(c, &avPacket, _frame, &got_output);
    if (ret < 0) {
        EXCLOG(LOG_ERROR, "Error encoding frame");
        exit(1);
    }

    if (got_output) {
        exstring fname;
        fname.format("img_%lld_%d.jpg", _frame->pts, ++index);
        EXCLOG(LOG_INFO, "file %s created!", fname.to_string().c_str());
        FILE* f = fopen(fname, "wb");
        fwrite(avPacket.data, 1, avPacket.size, f);
        av_free_packet(&avPacket);
        return true;
    }

    return false;
}

void Player::play(const char* filename)
{
    int ret;
    int got_frame;
    uint8_t* video_dst_data[4] = {nullptr,};
    int video_dst_linesize[4] = {0,};

    EXCLOG(LOG_INFO, "Player=%p", this);

    av_register_all();

    if (avformat_open_input(&_fmt_ctx, filename, /*input format : autoDetect*/ nullptr, nullptr) < 0) {
        EXCLOG(LOG_ERROR, "Could not open source file %s", filename);
        return;
    }

    // retrieve stream information
    if (avformat_find_stream_info(_fmt_ctx, nullptr) < 0) {
        EXCLOG(LOG_ERROR, "Could not find stream information");
        return;
    }

    // video codec init :
    if (open_codec_context(&_video_stream_idx, _fmt_ctx, AVMEDIA_TYPE_VIDEO) >= 0) {
        _video_stream = _fmt_ctx->streams[_video_stream_idx];
        _video_dec_ctx = _video_stream->codec;

        // allocate image where the decoded image will be put
        ret = av_image_alloc(video_dst_data, video_dst_linesize,
                             _video_dec_ctx->width, _video_dec_ctx->height,
                             _video_dec_ctx->pix_fmt, 1);
        if (ret < 0) {
            EXCLOG(LOG_ERROR, "Could not allocate raw video buffer");
            goto end;
        }
    }

    // audio codec init :
    if (open_codec_context(&_audio_stream_idx, _fmt_ctx, AVMEDIA_TYPE_AUDIO) >= 0) {
        _audio_stream = _fmt_ctx->streams[_audio_stream_idx];
        _audio_dec_ctx = _audio_stream->codec;
        EXCLOG(LOG_INFO, "_audio_stream_idx=%d codec=%s samplerate=%d format=%d", \
                _audio_stream_idx, _audio_dec_ctx->codec->name, _audio_dec_ctx->sample_rate, _audio_dec_ctx->sample_fmt);
    }

    SDLDisplay::get()->createDisplay(_video_dec_ctx->width, _video_dec_ctx->height);

    // initialize SWS context for software scaling
    _sws_ctx = sws_getContext(_video_dec_ctx->width, _video_dec_ctx->height,
            _video_dec_ctx->pix_fmt, _video_dec_ctx->width, _video_dec_ctx->height,
            PIX_FMT_YUV420P,
            SWS_BILINEAR,
            NULL,
            NULL,
            NULL);


    // set up YV12 pixel array (12 bits per pixel)
    _yPlaneSz = _video_dec_ctx->width * _video_dec_ctx->height;
    _uvPlaneSz = _video_dec_ctx->width * _video_dec_ctx->height / 4;
    _yPlane = (Uint8*)malloc(_yPlaneSz);
    _uPlane = (Uint8*)malloc(_uvPlaneSz);
    _vPlane = (Uint8*)malloc(_uvPlaneSz);
    if (!_yPlane || !_uPlane || !_vPlane) {
        fprintf(stderr, "Could not allocate pixel buffers - exiting\n");
        exit(1);
    }
    _uvPitch = _video_dec_ctx->width / 2;


    // SDL AUDIO INIT:
    SDLDisplay::get()->openAudio(_audio_dec_ctx->sample_rate, AUDIO_S16SYS, _audio_dec_ctx->channels, \
            AUDIO_SAMPLES, audio_callback, this);

    if (AV_SAMPLE_FMT_S16 != _audio_dec_ctx->sample_fmt) {
        _needAudioConverting = true;
    }

    if (_needAudioConverting) {
        // audio resampling init:
        _audio_swr = swr_alloc();
        av_opt_set_int(_audio_swr, "in_channel_layout",  _audio_dec_ctx->channel_layout, 0);
        av_opt_set_int(_audio_swr, "out_channel_layout", _audio_dec_ctx->channel_layout,  0);
        av_opt_set_int(_audio_swr, "in_sample_rate",     _audio_dec_ctx->sample_rate, 0);
        av_opt_set_int(_audio_swr, "out_sample_rate",    _audio_dec_ctx->sample_rate, 0);
        av_opt_set_sample_fmt(_audio_swr, "in_sample_fmt",  _audio_dec_ctx->sample_fmt, 0);
        av_opt_set_sample_fmt(_audio_swr, "out_sample_fmt", AV_SAMPLE_FMT_S16,  0);
        swr_init(_audio_swr);

        ret = av_samples_alloc_array_and_samples(&_audio_outputBuffer, &_audio_dst_linesize, _audio_dec_ctx->channels,
                _audio_dec_ctx->sample_rate, AV_SAMPLE_FMT_S16, 0);
        if (ret < 0) {
            fprintf(stderr, "Could not allocate destination samples\n");
            goto end;
        }
    }

    // dump input information to stderr
    av_dump_format(_fmt_ctx, 0, filename, 0);

    _frame = av_frame_alloc();
    if (!_frame) {
        EXCLOG(LOG_ERROR, "Could not allocate frame");
        ret = AVERROR(ENOMEM);
        goto end;
    }

    _aFrame = av_frame_alloc();
    if (!_aFrame) {
        EXCLOG(LOG_ERROR, "Could not allocate frame");
        ret = AVERROR(ENOMEM);
        goto end;
    }

    // initialize packet, set data to NULL, let the demuxer fill it
    av_init_packet(&_pkt);
    _pkt.data = nullptr;
    _pkt.size = 0;

    // read frames from the file
    while (av_read_frame(_fmt_ctx, &_pkt) >= 0) {
        AVPacket orig_pkt = _pkt;
        //EXCLOG(LOG_INFO, "stream index:%u pts:%llu size:%d\n", _pkt.stream_index, _pkt.pts, _pkt.size);
        do {
            ret = decode_packet(&got_frame);
            if (ret < 0)
                break;
            _pkt.data += ret;
            _pkt.size -= ret;
        } while (_pkt.size > 0);
        av_free_packet(&orig_pkt);
    }

    // flush cached frames
    _pkt.data = nullptr;
    _pkt.size = 0;
    do {
        decode_packet(&got_frame);
    } while (got_frame);
    EXCLOG(LOG_INFO, "decoding finished.");
end:
    return;
}

void Player::addPlayerListener(const PlayerListener* listener)
{
    list<PlayerListener*>::iterator itr = find_if(\
                _playerListenerList.begin(), _playerListenerList.end(), \
                [&](const PlayerListener* l){ return listener == l; });
    if (itr == _playerListenerList.end()) {
        _playerListenerList.push_back(const_cast<PlayerListener*>(listener));
    }
}

void Player::audioCallback(unsigned char* stream, int len)
{
    while(!_audioStreamQueue.get(stream, len)) {
        exthread::sleep(10);
    }
    //EXCLOG(LOG_INFO, "audio stream len=%d finished!", len);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void audio_callback (void *userdata, unsigned char* stream, int len)
{
    //EXCLOG(LOG_INFO, "audio stream len=%d userdata=%p", len, userdata);
    Player* player = (Player*)userdata;
    player->audioCallback(stream, len);
}

