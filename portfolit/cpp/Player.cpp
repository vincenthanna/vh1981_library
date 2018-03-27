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
    fmt_ctx(nullptr),
    video_stream_idx(-1), audio_stream_idx(-1),
    video_dec_ctx(nullptr), audio_dec_ctx(nullptr),
    video_stream(nullptr), audio_stream(nullptr),
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

void Player::pgm_save(unsigned char *buf, int wrap, int xsize, int ysize,
                     char *filename)
{
    FILE *f;
    int i;
    f = fopen(filename,"w");
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}

int Player::decode_packet(int *got_frame, int cached)
{
    int ret = 0;
    int decoded = pkt.size;
    if (pkt.stream_index == video_stream_idx) {
        /* decode video frame */
        ret = avcodec_decode_video2(video_dec_ctx, frame, got_frame, &pkt);
        if (ret < 0) {
            fprintf(stderr, "Error decoding video frame\n");
            return ret;
        }

        if (*got_frame) {
            printf("video_frame%s n:%d coded_n:%d size:%d format:%d (%d X %d) \n",
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

            if (result.detected) {
                WriteJPEG(video_dec_ctx, frame, frame->pts);
            }
        }
    } else if (pkt.stream_index == audio_stream_idx) {
        /* decode audio frame */
        ret = avcodec_decode_audio4(audio_dec_ctx, frame, got_frame, &pkt);
        if (ret < 0) {
            fprintf(stderr, "Error decoding audio frame\n");
            return ret;
        }
        /* Some audio decoders decode only part of the packet, and have to be
         * called again with the remainder of the packet data.
         * Sample: fate-suite/lossless-audio/luckynight-partial.shn
         * Also, some decoders might over-read the packet. */
        decoded = FFMIN(ret, pkt.size);
        if (*got_frame) {
            AVSampleFormat fmt = (AVSampleFormat)frame->format;
            size_t unpadded_linesize = frame->nb_samples * av_get_bytes_per_sample(fmt);
            printf("audio_frame%s n:%d nb_samples:%d\n",
                   cached ? "(cached)" : "",
                   audio_frame_count++, frame->nb_samples);
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
        fprintf(stderr, "Error sending a packet for decoding\n");
        exit(1);
    }
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error during decoding\n");
            exit(1);
        }
        printf("saving frame %3d\n", dec_ctx->frame_number);
        fflush(stdout);
        /* the picture is allocated by the decoder. no need to
         free it */
        snprintf(buf, sizeof(buf), "%s-%d", filename, dec_ctx->frame_number);
        pgm_save(frame->data[0], frame->linesize[0],
                 frame->width, frame->height, buf);
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
        fprintf(stderr, "Could not find %s stream in input file '%s'\n",
                av_get_media_type_string(type), src_filename);
        return ret;
    } else {
        *stream_idx = ret;
        st = fmt_ctx->streams[*stream_idx];
        /* find decoder for the stream */
        dec_ctx = st->codec;
        dec = avcodec_find_decoder(dec_ctx->codec_id);
        if (!dec) {
            fprintf(stderr, "Failed to find %s codec\n",
                    av_get_media_type_string(type));
            return ret;
        }
        if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0) {
            fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
            return ret;
        }
    }
    return 0;
}

int Player::WriteJPEG(AVCodecContext *pCodecCtx, AVFrame *pFrame, int FrameNo)
{
    int got_output = 0;
    int ret;
    AVPacket avPacket;

    av_init_packet(&avPacket);
    avPacket.data = NULL;
    avPacket.size = 0;

    AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
    if (!codec)
    {
        printf("Codec not found\n");
        exit(1);
    }

    AVCodecContext* c = avcodec_alloc_context3(codec);
    if (!c)
    {
        printf("Could not allocate video codec context\n");
        exit(1);
    }

    c->bit_rate = 400000;
    c->width = frame->width;
    c->height = frame->height;
    c->time_base= (AVRational){1,25};
    c->pix_fmt = AV_PIX_FMT_YUVJ420P;

    if (avcodec_open2(c, codec, NULL) < 0)
    {
        printf("Could not open codec\n");
        exit(1);
    }


    ret = avcodec_encode_video2(c, &avPacket, frame, &got_output);
    if (ret < 0)
    {
        printf("Error encoding frame\n");
        exit(1);
    }

    if (got_output)
    {
        printf("got frame\n");
        char fname[100];
        memset(fname, 0x0, sizeof(fname));
        sprintf(fname, "img_%d.jpg", frame->pts);
        EXCLOG(LOG_FATAL, "file %s created!", fname);
        FILE* f = fopen(fname, "wb");
        fwrite(avPacket.data, 1, avPacket.size, f);
        av_free_packet(&avPacket);
    }
}

void Player::play(const char* filename)
{
    int ret;
    int got_frame;

    // init motionOptions:
    motionOptions.minBlocks = 4;
    motionOptions.motionMode = 0; // not used
    motionOptions.motionSensitivity = 4;

    motionSearch.setDelegate(this);

    if (avformat_open_input(&fmt_ctx, filename, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open source file %s\n", filename);
        exit(1);
    }
    /* retrieve stream information */
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        exit(1);
    }

    if (open_codec_context(&video_stream_idx, fmt_ctx, AVMEDIA_TYPE_VIDEO, filename) >= 0) {
        video_stream = fmt_ctx->streams[video_stream_idx];
        video_dec_ctx = video_stream->codec;

        /* allocate image where the decoded image will be put */
        ret = av_image_alloc(video_dst_data, video_dst_linesize,
                             video_dec_ctx->width, video_dec_ctx->height,
                             video_dec_ctx->pix_fmt, 1);
        if (ret < 0) {
            fprintf(stderr, "Could not allocate raw video buffer\n");
            goto end;
        }
        //video_dst_bufsize = ret;
    }

    /* dump input information to stderr */
    av_dump_format(fmt_ctx, 0, filename, 0);

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate frame\n");
        ret = AVERROR(ENOMEM);
        goto end;
    }

    /* initialize packet, set data to NULL, let the demuxer fill it */
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
//    if (video_stream)
//        printf("Demuxing video from file '%s' into '%s'\n", filename, outfilename);

    /* read frames from the file */
    while (av_read_frame(fmt_ctx, &pkt) >= 0) {
        AVPacket orig_pkt = pkt;
        //printf("stream index:%u pts:%llu size:%d\n", pkt.stream_index, pkt.pts, pkt.size);
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
    EXCLOG(LOG_INFO, "Demuxing succeeded.");
end:
    return;
}


