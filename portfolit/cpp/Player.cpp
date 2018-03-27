#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "Player"
#endif

#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"swresample.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"avutil.lib")

#include <stdio.h>
#include <iostream>

#include "Player.h"

#include "library/basic/extools.h"
#include "library/basic/exlog.h"

#include "MotionSearch.h"

using namespace std;
using namespace VideoAnalytics;
using namespace vh1981lib;

Player::Player() : motionSearch(),
    _fmt_ctx(nullptr),
    _video_stream_idx(-1),
    _video_dec_ctx(nullptr),
    _video_stream(nullptr),
    frame(nullptr),
    motionOptions(),
    motionNew(true),
    video_frame_count(0), audio_frame_count(0)
{
    array_access(video_dst_data, i) {
        video_dst_data[i] = nullptr;
    }

    array_access(video_dst_linesize, i) {
        video_dst_linesize[i] = 0;
    }
}

void Player::getBlockAvg(const unsigned char *image,
                                     MotionBlockObject *motionAvgData,
                                     unsigned int width,
                                     unsigned int height,
                                     unsigned int stride)
{
    unsigned int blockWidth = (width) / (MOTION_SEARCH_COLUMN_COUNT);
    unsigned int blockHeight =(height) / (MOTION_SEARCH_ROW_COUNT);

    int blk_row,blk_col,blk_pos;

    for (blk_row = 0; blk_row < MOTION_SEARCH_ROW_COUNT; blk_row++) {
        for (blk_col = 0; blk_col < MOTION_SEARCH_COLUMN_COUNT; blk_col++) {

            blk_pos = blk_row * MOTION_SEARCH_COLUMN_COUNT + blk_col;

            unsigned int pixelpos = \
            stride * ((height * blk_row) / MOTION_SEARCH_ROW_COUNT) +   // y축 끝.
            stride * (blockHeight / 2) +                                // y축 offset.
            (width * blk_col) / MOTION_SEARCH_COLUMN_COUNT +            // x축 끝.
            (blockWidth / 2);                                           // x축 offset.

            motionAvgData[blk_pos].pixelData = (unsigned char)image[pixelpos];
        }
    }
    return;
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
            EXCLOG(LOG_INFO, "video_frame%s n:%d coded_n:%d size:%d format:%d (%d X %d)",
                   cached ? "(cached)" : "",
                   video_frame_count++, frame->coded_picture_number,
                   frame->pkt_size,
                   frame->format,
                   frame->width, frame->height);

            exstring str = "linesize:";
            for (int i = 0; i < 8; i++) {
                str.appendf("%d ", frame->linesize[i]);
            }

            str += " buf : ";
            for (int i = 0; i < AV_NUM_DATA_POINTERS; i++) {
                str.appendf("%p ", frame->data[i]);
            }

            str.append("\n");
            EXCLOG(LOG_INFO, str);

            MotionResult result;
            motionSearch.doMotionSearch(frame->data[0],
                                        frame->width,
                                        frame->height,
                                        frame->linesize[0],
                                        MOTION_SEARCH_ROW_COUNT,
                                        MOTION_SEARCH_COLUMN_COUNT,
                                        &motionOptions,
                                        motionNew,
                                        frame->pts,
                                        &result);

            if (motionNew) {
                motionNew = false;
            }

            // 움직임이 감지된 이미지를 파일로 출력한다.
            if (result.detected) {
                result.time = frame->pts;
                _motionsDetected.push_back(result);
                WriteJPEG(_video_dec_ctx, frame, frame->pts);
            }
        }
    }
    return decoded;
}

void Player::decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt, char *filename)
{
    char buf[1024];
    int ret;
    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        EXCLOG(LOG_ERROR, "Error sending a packet for decoding\n");
        exit(1);
    }
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return;
        }
        else if (ret < 0) {
            EXCLOG(LOG_ERROR, "Error during decoding");
            exit(1);
        }

        EXCLOG(LOG_INFO, "saving frame %3d", dec_ctx->frame_number);
    }
}

int Player::open_codec_context(int *stream_idx, AVFormatContext *fmt_ctx, enum AVMediaType type, const char* src_filename)
{
    int ret;
    AVStream *st;
    AVCodecContext *dec_ctx = NULL;
    AVCodec *dec = NULL;
    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0) {
        EXCLOG(LOG_ERROR, "Could not find %s stream in input file '%s'",
               av_get_media_type_string(type), src_filename);
        return ret;
    }
    else {
        *stream_idx = ret;
        st = fmt_ctx->streams[*stream_idx];
        /* find decoder for the stream */
        dec_ctx = st->codec;
        dec = avcodec_find_decoder(dec_ctx->codec_id);
        if (!dec) {
            EXCLOG(LOG_ERROR, "Failed to find %s codec", av_get_media_type_string(type));
            return ret;
        }
        if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0) {
            EXCLOG(LOG_ERROR, "Failed to open %s codec", av_get_media_type_string(type));
            return ret;
        }
    }
    return 0;
}

bool Player::WriteJPEG(AVCodecContext *pCodecCtx, AVFrame *pFrame, int FrameNo)
{
    int got_output = 0;
    int ret;
    AVPacket avPacket;

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

    c->bit_rate = 400000;
    c->width = frame->width;
    c->height = frame->height;
    c->time_base= (AVRational){1,25};
    c->pix_fmt = AV_PIX_FMT_YUVJ420P;

    if (avcodec_open2(c, codec, NULL) < 0) {
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
        fname.format("img_%lld.jpg", frame->pts);
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

    // init motionOptions:
    motionOptions.minBlocks = 4;
    motionOptions.motionMode = 0; // not used
    motionOptions.motionSensitivity = 4;

    _motionsDetected.clear();

    motionSearch.setDelegate(this);

    if (avformat_open_input(&_fmt_ctx, filename, NULL, NULL) < 0) {
        EXCLOG(LOG_ERROR, "Could not open source file %s", filename);
        exit(1);
    }
    /* retrieve stream information */
    if (avformat_find_stream_info(_fmt_ctx, NULL) < 0) {
        EXCLOG(LOG_ERROR, "Could not find stream information");
        exit(1);
    }

    if (open_codec_context(&_video_stream_idx, _fmt_ctx, AVMEDIA_TYPE_VIDEO, filename) >= 0) {
        _video_stream = _fmt_ctx->streams[_video_stream_idx];
        _video_dec_ctx = _video_stream->codec;

        /* allocate image where the decoded image will be put */
        ret = av_image_alloc(video_dst_data, video_dst_linesize,
                             _video_dec_ctx->width, _video_dec_ctx->height,
                             _video_dec_ctx->pix_fmt, 1);
        if (ret < 0) {
            EXCLOG(LOG_ERROR, "Could not allocate raw video buffer");
            goto end;
        }
    }

    /* dump input information to stderr */
    av_dump_format(_fmt_ctx, 0, filename, 0);

    frame = av_frame_alloc();
    if (!frame) {
        EXCLOG(LOG_ERROR, "Could not allocate frame");
        ret = AVERROR(ENOMEM);
        goto end;
    }

    /* initialize packet, set data to NULL, let the demuxer fill it */
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    /* read frames from the file */
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
    /* flush cached frames */
    pkt.data = NULL;
    pkt.size = 0;
    do {
        decode_packet(&got_frame, 1);
    } while (got_frame);
    EXCLOG(LOG_INFO, "decoding finished.");

    EXCLOG(LOG_INFO, "Motions Settings : MinBlock %d Sensitivity %d",
           motionOptions.minBlocks, motionOptions.motionSensitivity);
    EXCLOG(LOG_INFO, "Detected Motions(%d):", _motionsDetected.size());
    for (auto result : _motionsDetected) {
        EXCLOG(LOG_INFO, "\tdetected pts:%llu", result.time);
    }
end:
    return;
}


