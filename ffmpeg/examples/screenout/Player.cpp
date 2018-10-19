#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "Player"
#endif


#include "Player.h"

#include "library/basic/extools.h"
#include "library/basic/exlog.h"
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

using namespace vh1981lib;

Player::Player() : _playerListenerList()
    , _fmt_ctx(nullptr)
{

}

bool Player::convertYuvToRgb(const AVFrame* srcFrame, AVFrame* dstFrame, AVPixelFormat dstFmt)
{
    struct SwsContext* sws_ctx = nullptr;
    sws_ctx = sws_getContext(
                _video_dec_ctx->width,
                _video_dec_ctx->height,
                (AVPixelFormat)dstFmt,
                srcFrame->width,
                srcFrame->height,
                (AVPixelFormat)dstFmt,
                SWS_BILINEAR, nullptr, nullptr, nullptr);

    int num_bytes = avpicture_get_size(dstFmt, _video_dec_ctx->width, _video_dec_ctx->height);

    uint8_t* buffer=(uint8_t *)av_malloc(num_bytes*sizeof(uint8_t));

    int ret = avpicture_fill((AVPicture *)dstFrame, buffer, dstFmt, _video_dec_ctx->width, _video_dec_ctx->height);
    cout << "avpicture_fill() ret=" << ret << endl;

    dstFrame->width = srcFrame->width;
    dstFrame->height = srcFrame->height;
    dstFrame->format = dstFmt;

    cout << __LINE__ << "dst : " << dstFrame->width << " " << dstFrame->height << " " << endl;
    ret = sws_scale(sws_ctx, srcFrame->data, srcFrame->linesize, 0, _video_dec_ctx->height,
              ((AVPicture*)dstFrame)->data, ((AVPicture*)dstFrame)->linesize);

    cout << "sws_scale() return [" << ret << "]" << endl;


}


int Player::decode_packet(int *got_frame, int cached)
{
    int ret = 0;
    int decoded = pkt.size;
    if (pkt.stream_index == _video_stream_idx) {
        /* decode video frame */
        ret = avcodec_decode_video2(_video_dec_ctx, frame, got_frame, &pkt);
        if (ret < 0) {
            EXCLOG(LOG_ERROR, "Error decoding video frame");
            return ret;
        }

        if (*got_frame) {
            EXCLOG(LOG_INFO, "video_frame%s coded_n:%d size:%d format:%d (%d X %d)", \
                   cached ? "(cached)" : "", frame->coded_picture_number, frame->pkt_size, \
                   frame->format,frame->width, frame->height);



            // FIXME: todo

            AVPicture pict;
            pict.data[0] = yPlane;
            pict.data[1] = uPlane;
            pict.data[2] = vPlane;
            pict.linesize[0] = _video_dec_ctx->width;
            pict.linesize[1] = uvPitch;
            pict.linesize[2] = uvPitch;

            //EXCLOG(LOG_INFO, "TRACE");

            // Convert the image into YUV format that SDL uses
            sws_scale(sws_ctx, (uint8_t const * const *) frame->data,
                    frame->linesize, 0, _video_dec_ctx->height, pict.data,
                    pict.linesize);
            //EXCLOG(LOG_INFO, "TRACE");

            SDLDisplay::get()->updateTexture(yPlane, _video_dec_ctx->width, uPlane, uvPitch, vPlane, uvPitch);

            //EXCLOG(LOG_INFO, "TRACE");


            for (auto listener : _playerListenerList) {
                listener->FrameReady(frame);
            }
            //EXCLOG(LOG_INFO, "TRACE");
        }
    }
    return decoded;
}

int Player::open_codec_context(int *stream_idx, AVFormatContext *fmt_ctx, enum AVMediaType type, const char* src_filename)
{
    int ret;
    AVStream *st;
    AVCodecContext *dec_ctx = nullptr;
    AVCodec *dec = nullptr;
    ret = av_find_best_stream(fmt_ctx, type, -1, -1, nullptr, 0);
    if (ret < 0) {
        EXCLOG(LOG_ERROR, "Could not find %s stream in input file '%s'",
               av_get_media_type_string(type), src_filename);
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

    ret = avcodec_encode_video2(c, &avPacket, frame, &got_output);
    if (ret < 0) {
        EXCLOG(LOG_ERROR, "Error encoding frame");
        exit(1);
    }

    if (got_output) {
        exstring fname;
        fname.format("img_%lld_%d.jpg", frame->pts, ++index);
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

    if (open_codec_context(&_video_stream_idx, _fmt_ctx, AVMEDIA_TYPE_VIDEO, filename) >= 0) {
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


    SDLDisplay::get()->createDisplay(_video_dec_ctx->width, _video_dec_ctx->height);

    // initialize SWS context for software scaling
    sws_ctx = sws_getContext(_video_dec_ctx->width, _video_dec_ctx->height,
            _video_dec_ctx->pix_fmt, _video_dec_ctx->width, _video_dec_ctx->height,
            PIX_FMT_YUV420P,
            SWS_BILINEAR,
            NULL,
            NULL,
            NULL);

    // set up YV12 pixel array (12 bits per pixel)
    yPlaneSz = _video_dec_ctx->width * _video_dec_ctx->height;
    uvPlaneSz = _video_dec_ctx->width * _video_dec_ctx->height / 4;
    yPlane = (Uint8*)malloc(yPlaneSz);
    uPlane = (Uint8*)malloc(uvPlaneSz);
    vPlane = (Uint8*)malloc(uvPlaneSz);
    if (!yPlane || !uPlane || !vPlane) {
        fprintf(stderr, "Could not allocate pixel buffers - exiting\n");
        exit(1);
    }
    uvPitch = _video_dec_ctx->width / 2;




    // dump input information to stderr
    av_dump_format(_fmt_ctx, 0, filename, 0);

    frame = av_frame_alloc();
    if (!frame) {
        EXCLOG(LOG_ERROR, "Could not allocate frame");
        ret = AVERROR(ENOMEM);
        goto end;
    }

    // initialize packet, set data to NULL, let the demuxer fill it
    av_init_packet(&pkt);
    pkt.data = nullptr;
    pkt.size = 0;

    // read frames from the file
    while (av_read_frame(_fmt_ctx, &pkt) >= 0) {
        AVPacket orig_pkt = pkt;
        EXCLOG(LOG_INFO, "stream index:%u pts:%llu size:%d\n", pkt.stream_index, pkt.pts, pkt.size);
        do {
            ret = decode_packet(&got_frame, 0);
            if (ret < 0)
                break;
            pkt.data += ret;
            pkt.size -= ret;
        } while (pkt.size > 0);
        av_free_packet(&orig_pkt);
    }

    // flush cached frames
    pkt.data = nullptr;
    pkt.size = 0;
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
