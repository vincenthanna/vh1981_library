#ifndef _VH1981_PLAYER_
#define _VH1981_PLAYER_

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

#include "MotionSearch.h"

namespace VideoAnalytics {
    /**
     @class Player
     @brief ffmpeg으로 영상을 디코딩하는 기능을 구현
     */

	class Player : MotionBlockAvgDelegate {
        /**
         @name 생성자/소멸자
         */
        //@{
        Player();

        virtual ~Player() {}
        //@}

        /**
         @name 속성
         */
        //@{
        enum {
            INBUF_SIZE = 4096,
        }

        MotionSearch motionSearch;

        AVFormatContext *fmt_ctx;
        int video_stream_idx;
        int audio_stream_idx;
        AVCodecContext *video_dec_ctx, *audio_dec_ctx;
        AVStream *video_stream, *audio_stream;
        //char* video_dst_filename;
//        FILE *video_dst_file;
//        FILE *audio_dst_file;
        uint8_t *video_dst_data[4] = {NULL};
        int      video_dst_linesize[4];
//        int video_dst_bufsize;
        AVPacket pkt;
        AVFrame *frame;

        MCP_MOTION_OPTIONS motionOptions;
        bool motionNew;

        int video_frame_count;
        int audio_frame_count;

        //@}

        /**
         @name 디코딩 함수
         */
        //@{
        void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize, char *filename);
        int decode_packet(int *got_frame, int cached);
        void decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt, char *filename);
        int open_codec_context(int *stream_idx, AVFormatContext *fmt_ctx, \
                               enum AVMediaType type, const char* src_filename);
        void play(const char* filename); // FIXME: main()함수를 정리할 것
        //@}

        /**
         @name MotionBlockAvgDelegate 구현
         */
        //@{
        void getBlockAvg(const unsigned char *image,
                                             MotionBlockObject *motionAvgData,
                                             unsigned int width,
                                             unsigned int height,
                                             unsigned int stride);
        //@}


	};
}

#endif
