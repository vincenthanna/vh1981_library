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

#include <vector>

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
	public:
        Player();

        virtual ~Player() {}
    //@}

    /**
     @name 속성
     */
    //@{
        enum {
            INBUF_SIZE = 4096,
        };
    private:
        MotionSearch _motionSearch;

        AVFormatContext* _fmt_ctx;
        int _video_stream_idx;
        AVCodecContext* _video_dec_ctx;
        AVStream* _video_stream;
        AVPacket pkt;
        AVFrame *frame;
        MotionOptions _motionOptions;
        bool _motionNew;

        std::vector<MotionResult> _motionsDetected;
    //@}

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
        bool writeJPEG (AVCodecContext *pCodecCtx, AVFrame *pFrame, int FrameNo);
     //@}

    /**
     @name MotionBlockAvgDelegate 구현
     */
    //@{
        void getBlockAvg(const unsigned char *image, MotionBlockObject *motionAvgData, unsigned int width, \
            unsigned int height, unsigned int stride);
    //@}

    /**
     @name 외부 인터페이스
     */
    //@{
     public:
        void play(const char* filename, MotionOptions motionOptions);
    //@}


	};
}

#endif
