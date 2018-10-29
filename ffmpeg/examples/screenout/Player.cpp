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
#define MAX_AUDIO_FRAME_SIZE 192000

using namespace vh1981lib;

Player::Player() : _playerListenerList()
    , _fmt_ctx(nullptr)
    , _audio_dec_ctx(nullptr)
    ,_sws_ctx(nullptr)
//    ,_audioPacketQueue()
    ,_audio_buf_size(0)
    ,_audio_buf_index(0)
    ,_audio_pkt_data(nullptr)
    ,_audio_pkt_size(0)
    ,_audioStreamQueue(1024 * 128)
{

}

int Player::decode_packet(int *got_frame, int cached)
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

            //EXCLOG(LOG_INFO, "TRACE");

            // Convert the image into YUV format that SDL uses
            sws_scale(_sws_ctx, (uint8_t const * const *) _frame->data,
                    _frame->linesize, 0, _video_dec_ctx->height, pict.data,
                    pict.linesize);
            //EXCLOG(LOG_INFO, "TRACE");

            SDLDisplay::get()->updateTexture(_yPlane, _video_dec_ctx->width, _uPlane, _uvPitch, _vPlane, _uvPitch);

            //EXCLOG(LOG_INFO, "TRACE");


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
#if 1
            if (got_frame) {

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
                //EXCLOG(LOG_INFO, "audio data size : %d", dst_bufsize);

                while (!_audioStreamQueue.put(_audio_outputBuffer[0], dst_bufsize)) {
                    exthread::sleep(10);
                }
            }
#endif

#if 0
            if (got_frame) {
                // SDL out only
                int dataSize = av_samples_get_buffer_size(NULL, _audio_dec_ctx->channels, _aFrame->nb_samples, \
                        _audio_dec_ctx->sample_fmt, 1);
                while (!_audioStreamQueue.put(_aFrame->data[0], dataSize)) {
                    exthread::sleep(10);
                }
            }
#endif
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
    SDL_AudioSpec specSrc, spec;
    specSrc.freq = _audio_dec_ctx->sample_rate;
    specSrc.format = AUDIO_S16SYS;
    specSrc.channels = _audio_dec_ctx->channels;
    specSrc.silence = 0;
    specSrc.samples = SDL_AUDIO_BUFFER_SIZE;
    specSrc.callback = audio_callback;
    specSrc.userdata = this;
    if(SDL_OpenAudio(&specSrc, &spec) < 0) {
      //fprintf(stderr, "SDL_OpenAudio: %s\n", SDL_GetError());
        EXCLOG(LOG_FATAL, "SDL_OpenAudio: %s", SDL_GetError());
        exit(-1);
    }

    // SDL audio 동작 start. audio_callback이 호출되기 시작한다.
    SDL_PauseAudio(0); //< finally starts the audio device. it plays silence if it doesn't get data



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
            ret = decode_packet(&got_frame, 0);
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
        decode_packet(&got_frame, 1);
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

#if 0
int Player::audio_decode_frame(uint8_t *audio_buf, int buf_size)
{
//    static AVPacket pkt;
//    static uint8_t *audio_pkt_data = NULL;
//    static int audio_pkt_size = 0;
//    static AVFrame frame;

    int len1, data_size = 0;

    for(;;) {
        while(_audio_pkt_size > 0) {
            int got_frame = 0;
            len1 = avcodec_decode_audio4(_audio_dec_ctx, _aFrame, &got_frame, &_audioPkt);
            if(len1 < 0) {
                /* if error, skip frame */
                _audio_pkt_size = 0;
                break;
            }
            _audio_pkt_data += len1;
            _audio_pkt_size -= len1;
            data_size = 0;
            if(got_frame) {
                data_size = av_samples_get_buffer_size(NULL,
                        _audio_dec_ctx->channels,
                        _aFrame->nb_samples,
                        _audio_dec_ctx->sample_fmt,
                        1);
                assert(data_size <= buf_size);
                memcpy(audio_buf, _aFrame->data[0], data_size);
            }
            if(data_size <= 0) {
                /* No data yet, get more frames */
                continue;
            }
            /* We have data, return it and come back for more later */
            return data_size;
        }

//        if(_audioPkt.data) {
//            av_free_packet(&_audioPkt);
//        }

//        if(_quit) {
//            return -1;
//        }

        if(_audioPacketQueue.get(&_audioPkt, 1) < 0) {
            return -1;
        }
        _audio_pkt_data = _audioPkt.data;
        _audio_pkt_size = _audioPkt.size;
    }
}
#endif


#if 0
/////////////////////////////////////////////////////////////////////////////////////////////


Player::PacketQueue::PacketQueue() : _first_pkt(nullptr)
    ,_last_pkt(nullptr)
    ,_nb_packets(0)
    ,_size(0)
    ,_mutex(nullptr)
    ,_cond(nullptr)
    ,_quit(false)
{

}

void Player::PacketQueue::init()
{
    _mutex = SDL_CreateMutex();
    _cond = SDL_CreateCond();
}

int Player::PacketQueue::put(AVPacket *pkt) {

    AVPacketList *pkt1;
    if(av_dup_packet(pkt) < 0) {
        return -1;
    }
    pkt1 = (AVPacketList*)av_malloc(sizeof(AVPacketList));
    if (!pkt1)
        return -1;
    pkt1->pkt = *pkt;
    pkt1->next = NULL;


    SDL_LockMutex(_mutex);

    if (!_last_pkt) {
        _first_pkt = pkt1;
    }
    else {
        _last_pkt->next = pkt1;
    }

    _last_pkt = pkt1;
    _nb_packets++;
    _size += pkt1->pkt.size;
    SDL_CondSignal(_cond);
    SDL_UnlockMutex(_mutex);

    return 0;
}

int Player::PacketQueue::get(AVPacket *pkt, int block) {
  AVPacketList *pkt1;
  int ret;

  SDL_LockMutex(_mutex);

  for(;;) {

      if(_quit) {
          ret = -1;
          break;
      }

      pkt1 = _first_pkt;
      if (pkt1) {
          _first_pkt = pkt1->next;
          if (!_first_pkt) {
              _last_pkt = nullptr;
          }
          _nb_packets--;
          _size -= pkt1->pkt.size;
          *pkt = pkt1->pkt;
          av_free(pkt1);
          ret = 1;
          break;
      } else if (!block) {
          ret = 0;
          break;
      } else {
          SDL_CondWait(_cond, _mutex);
      }
  }
  SDL_UnlockMutex(_mutex);
  return ret;
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////


void audio_callback (void *userdata, unsigned char* stream, int len)
{
    //EXCLOG(LOG_INFO, "audio stream len=%d userdata=%p", len, userdata);
    Player* player = (Player*)userdata;
    player->audioCallback(stream, len);
}
