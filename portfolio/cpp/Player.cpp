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

Player::Player() : _motionSearch(),
    _fmt_ctx(nullptr),
    _video_stream_idx(-1),
    _video_dec_ctx(nullptr),
    _video_stream(nullptr),
    _motionOptions(),
    _motionNew(true)
{
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

int Player::decode_packet(const AVPacket& pkt, AVFrame *frame, int *got_frame, int cached)
{
    int ret = 0;
    int decoded = pkt.size;
    // video stream 처리 :
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
            _motionSearch.doMotionSearch(frame->data[0],
                                        frame->width,
                                        frame->height,
                                        frame->linesize[0],
                                        MOTION_SEARCH_ROW_COUNT,
                                        MOTION_SEARCH_COLUMN_COUNT,
                                        &_motionOptions,
                                        _motionNew,
                                        frame->pts,
                                        &result);

            if (_motionNew) {
                _motionNew = false;
            }

            // 움직임이 감지된 이미지를 파일로 출력한다.
            if (result.detected) {
                result.time = frame->pts;
                _motionsDetected.push_back(result);
                writeJPEG(frame, frame->pts);
            }
        }
    }
    return decoded;
}

bool Player::open_codec_context(int *stream_idx,
    AVFormatContext *fmt_ctx,
    enum AVMediaType type, // 오디오/비디오/데이터/자막... 등의 종류를 구분
    const char* src_filename)
{
    int ret;
    AVStream *st;
    AVCodecContext *dec_ctx = nullptr;
    AVCodec *dec = nullptr;
    ret = av_find_best_stream(fmt_ctx,
        type,
        -1, // 원하는 stream 번호, -1이면 autodetect
        -1, // ?? 모름
        nullptr, // 선택된 디코더를 얻고 싶으면 AVCodec**, 아니면 NULL.
        0); // flags. 지금은 사용되지 않고 있음.

    if (ret < 0) {
        EXCLOG(LOG_ERROR, "Could not find %s stream in input file '%s'",
               av_get_media_type_string(type), src_filename);
        return false;
    }
    else {
        *stream_idx = ret;
        st = fmt_ctx->streams[*stream_idx];

        // FIXME: 아래 코드는 그냥 열어보기만 하는듯. 동작에는 특별히 영향이 없다.
        //@{
        dec = avcodec_find_decoder(st->codecpar->codec_id);
        if (!dec) {
            EXCLOG(LOG_ERROR, "Failed to find %s codec", av_get_media_type_string(type));
            return false;
        }

        if ((ret = avcodec_open2(dec_ctx, dec, nullptr)) < 0) {
            EXCLOG(LOG_ERROR, "Failed to open %s codec", av_get_media_type_string(type));
            return false;
        }
        //@}
    }
    return ret;
}

bool Player::writeJPEG(AVFrame *pFrame, int FrameNo)
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

    c->bit_rate = 500000;
    c->width = pFrame->width;
    c->height = pFrame->height;
    c->time_base= (AVRational){1,25};
    c->pix_fmt = pFrame->format;

    if (avcodec_open2(c, codec, nullptr) < 0) {
        EXCLOG(LOG_ERROR, "Could not open codec");
        return false;
    }
    
    bool succeeded = false;

    ret = avcodec_encode_video2(c, &avPacket, pFrame, &got_output);
    if (ret < 0) {
        EXCLOG(LOG_ERROR, "Error encoding frame");
        exit(1);
    }

    if (got_output) {
        exstring fname;
        fname.format("img_%lld.jpg", pFrame->pts);
        EXCLOG(LOG_INFO, "file %s created!", fname.to_string().c_str());
        FILE* f = fopen(fname, "wb");
        fwrite(avPacket.data, 1, avPacket.size, f);
        av_free_packet(&avPacket);
        succeeded = true;
    }

    avcodec_free_context(&c);

    return succeeded;
}

void Player::play(const char* filename, MotionOptions motionOptions)
{
    int ret;
    int got_frame;
    uint8_t* video_dst_data[4] = {nullptr,};
    int video_dst_linesize[4] = {0,};

    _motionOptions = motionOptions;

    _motionsDetected.clear();

    _motionSearch.setDelegate(this);

    // 파일 경로로 context를 open한다.
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

        // 아래 코드도 필요없다.
#if 0
        // allocate image where the decoded image will be put
        ret = av_image_alloc(video_dst_data, video_dst_linesize,
                             _video_dec_ctx->width, _video_dec_ctx->height,
                             _video_dec_ctx->pix_fmt, 1);
#endif

        if (ret < 0) {
            EXCLOG(LOG_ERROR, "Could not allocate raw video buffer");
            goto end;
        }
    }
    else {
        EXCLOG(LOG_ERROR, "open_codec_context() failed!");
        goto end;
    }


    // dump input information to stderr
    av_dump_format(_fmt_ctx, 0, filename, 0);

    AVFrame* frame = av_frame_alloc();
    if (!frame) {
        EXCLOG(LOG_ERROR, "Could not allocate frame");
        ret = AVERROR(ENOMEM);
        goto end;
    }

    // initialize packet, set data to NULL, let the demuxer fill it
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = nullptr;
    pkt.size = 0;

    // read frames from the file
    while (av_read_frame(_fmt_ctx, &pkt) >= 0) {
        AVPacket orig_pkt = pkt;
        EXCLOG(LOG_INFO, "stream index:%u pts:%llu size:%d\n", pkt.stream_index, pkt.pts, pkt.size);
        // 읽어들인 pkt에서 1 or N개의 프레임을 읽어들인다.
        //@{
        do {
            ret = decode_packet(pkt, frame, &got_frame, 0);
            if (ret < 0)
                break;
            pkt.data += ret;
            pkt.size -= ret;
        } while (pkt.size > 0);
        //@}

        av_packet_unref(&orig_pkt); // AVPacket을 모두 사용하였으면 꼭 호출해 준다.
    }

    // flush cached frames
    //@{
    pkt.data = nullptr;
    pkt.size = 0;
    do {
        decode_packet(pkt, frame, &got_frame, 1);
    } while (got_frame);
    //@}

    av_frame_free(&frame); // AVFrame 할당한 버퍼를 해제한다.

    EXCLOG(LOG_INFO, "decoding finished.");

    EXCLOG(LOG_INFO, "Motions Settings : MinBlock %d Sensitivity %d",
           _motionOptions.minBlocks, _motionOptions.motionSensitivity);
    EXCLOG(LOG_INFO, "Detected Motions(%d):", _motionsDetected.size());
    for (auto result : _motionsDetected) {
        EXCLOG(LOG_INFO, "\tdetected pts:%llu", result.time);
    }
end:
    return;
}


