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

#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"swresample.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"avutil.lib")

#include <stdio.h>

#include <iostream>

#include "MotionSearch.h"

using namespace std;
using namespace VideoAnalytics;

MotionSearch motionSearch;

AVFormatContext *fmt_ctx = NULL;
int video_stream_idx = -1;
int audio_stream_idx = -1;
static AVCodecContext *video_dec_ctx = NULL, *audio_dec_ctx;
static AVStream *video_stream = NULL, *audio_stream = NULL;
char* video_dst_filename = NULL;
FILE *video_dst_file = NULL;
FILE *audio_dst_file = NULL;
uint8_t *video_dst_data[4] = {NULL};
int      video_dst_linesize[4];
int video_dst_bufsize;
AVPacket pkt;
AVFrame *frame;

MCP_MOTION_OPTIONS motionOptions;
bool motionNew = true;

int video_frame_count = 0;
int audio_frame_count = 0;

class MotionBlockAvgImpl : public MotionBlockAvgDelegate
{
public:
	MotionBlockAvgImpl();
	virtual ~MotionBlockAvgImpl() {}
public:
	/**
	 @name MotionSearch 인스턴스로 움직임 검색을 사용할 경우 아래 함수 구현을 제공해야 한다.
		   사용되는 프레임의 이미지 포맷이 YCbCr 4:2:2인지, YCbCr 4:2:0인지, RGB인지에 따라
		   픽셀을 뽑아내는 방법이 달라져야 함.

	 @param image           프레임 이미지
	 @param motionAvgData   averaged block value(Y).
	 @param width           프레임 width
	 @param height          프레임 height
	 @param stride          number of bytes from one row of pixels.
							in memory to the next row of pixels in memory.
	 @param motionMask      marked area to be searched.

	 @return void.
	 */
	//@{
	virtual void getBlockAvg(
			const unsigned char *image,
			MotionBlockObject *motionAvgData,
			unsigned int width,
			unsigned int height,
			unsigned int stride);
	//@}
};

MotionBlockAvgImpl::MotionBlockAvgImpl() : MotionBlockAvgDelegate()
{

}

void MotionBlockAvgImpl::getBlockAvg(const unsigned char *image,
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


//			uint32 pixelpos[MBO_PIXEL_COUNT];
//			uint32 pixelPosCount = 0;
//			for (int sbr = 1; sbr < (MBO_DETAIL_LEVEL + 1); sbr++) {
//				for (int sbc = 1; sbc < (MBO_DETAIL_LEVEL + 1); sbc++) {
//					pixelpos[pixelPosCount] =
//							stride * ((height * blk_row) / MOTION_SEARCH_ROW_COUNT) + // y축 끝.
//							stride * ((blockHeight * sbr) / (MBO_DETAIL_LEVEL + 1)) + // y축 offset.
//							(width * blk_col) / MOTION_SEARCH_COLUMN_COUNT +          // x축 끝.
//							(blockWidth * sbc) / (MBO_DETAIL_LEVEL + 1);              // x축 offset.
//
//					pixelPosCount++;
//				}
//			}

			unsigned int pixelpos = \
                                        stride * ((height * blk_row) / MOTION_SEARCH_ROW_COUNT) + // y축 끝.
										stride * (blockHeight / 2) + // y축 offset.
										(width * blk_col) / MOTION_SEARCH_COLUMN_COUNT +          // x축 끝.
										(blockWidth / 2);              // x축 offset.


//			for (int32 i = 0; i < pixelPosCount; i++) {
//				uint32 valueSum = 0;
//				uint32 samplingCount = 3;
//				for (uint32 off = 0; off < samplingCount; off++) {
//					valueSum += (image[pixelpos[i] + off]);
//					valueSum += (image[pixelpos[i] - off]);
//				}
//				motionAvgData[blk_pos].pixelData[i] = (unsigned char)(valueSum/(samplingCount*2));
//			}
			motionAvgData[blk_pos].pixelData = (unsigned char)image[pixelpos];

		}
	}
	return;
}

MotionBlockAvgImpl motionBlockAvgImpl;


#define INBUF_SIZE 4096
static void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize,
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

int decode_packet(int *got_frame, int cached)
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
            printf("video_frame%s n:%d coded_n:%d pts:%s size:%d format:%d (%d X %d) \n",
                   cached ? "(cached)" : "",
                   video_frame_count++, frame->coded_picture_number,
                   av_ts2timestr(frame->pts, &video_dec_ctx->time_base),
                   frame->pkt_size,
                   frame->format,
                   frame->width, frame->height);

            string str = "linesize:";
            for (int i = 0; i < 8; i++) {
                char buf[100];
                memset(buf, 0x0, sizeof(buf));
                sprintf(buf, "%d ", frame->linesize[i]);
                str.append(buf);
            }

            str = " buf : ";
            for (int i = 0; i < AV_NUM_DATA_POINTERS; i++) {
                char buf[100];
                memset(buf, 0x0, sizeof(buf));
                if (frame->data[i] != NULL) {
                    sprintf(buf, "%p ", frame->data[i]);
                }
                str.append(buf);
            }

            str.append("\n");
            printf("%s", str.c_str());

            // format == 0 : YCbCr 4:2:0
//            unsigned char *vo_buffer, // _video_out_buffer.
//                    unsigned int width,
//                    unsigned int height,
//                    unsigned int stride,
//                    unsigned int motionZoneRowCount,
//                    unsigned int motionZoneColCount,
//                    MCP_MOTION_OPTIONS* motionOptions, // it has motionMode, motionSensitivity, minBlocks, motionMask.
//                    bool motionNew,                    // motion search start or ongoing.
//                    uint32_t frameTime,                    // segmentId, time, tick needed when motion detected.
//                    unsigned int *detected,            // if detected, set to 1 inside.
//                    MotionResult *results


#if 1
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
#endif


			if (motionNew) {
				motionNew = false;
			}

            /* copy decoded frame to destination buffer:
             * this is required since rawvideo expects non aligned data */
            av_image_copy(video_dst_data, video_dst_linesize,
                          (const uint8_t **)(frame->data), frame->linesize,
                          video_dec_ctx->pix_fmt, video_dec_ctx->width, video_dec_ctx->height);
            /* write to rawvideo file */
            fwrite(video_dst_data[0], 1, video_dst_bufsize, video_dst_file);
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
            printf("audio_frame%s n:%d nb_samples:%d pts:%s\n",
                   cached ? "(cached)" : "",
                   audio_frame_count++, frame->nb_samples,
                   av_ts2timestr(frame->pts, &audio_dec_ctx->time_base));
            /* Write the raw audio data samples of the first plane. This works
             * fine for packed formats (e.g. AV_SAMPLE_FMT_S16). However,
             * most audio decoders output planar audio, which uses a separate
             * plane of audio samples for each channel (e.g. AV_SAMPLE_FMT_S16P).
             * In other words, this code will write only the first audio channel
             * in these cases.
             * You should use libswresample or libavfilter to convert the frame
             * to packed data. */
            fwrite(frame->extended_data[0], 1, unpadded_linesize, audio_dst_file);
        }
    }
    return decoded;
}

static void decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt, char *filename)
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

static int open_codec_context(int *stream_idx, AVFormatContext *fmt_ctx, enum AVMediaType type, const char* src_filename)
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

int main(int argc, char** argv)
{   
    char *filename, *outfilename;
    const AVCodec *codec;
    AVCodecParserContext *parser;
    AVCodecContext *c= NULL;
    FILE *f;
    uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    uint8_t *data;
    size_t   data_size;
    int ret;
    int got_frame;
    
    if (argc <= 2) {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        exit(0);
    }
    
    filename    = argv[1];
    outfilename = argv[2];

    // init motionOptions:
    motionOptions.minBlocks = 10;
    motionOptions.motionMode = 0; // not used
    motionOptions.motionSensitivity = 2;
    
    motionSearch.setDelegate(&motionBlockAvgImpl);

    video_dst_filename = outfilename;
    
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
        video_dst_file = fopen(video_dst_filename, "wb");
        if (!video_dst_file) {
            fprintf(stderr, "Could not open destination file %s\n", video_dst_filename);
            ret = 1;
            goto end;
        }
        /* allocate image where the decoded image will be put */
        ret = av_image_alloc(video_dst_data, video_dst_linesize,
                             video_dec_ctx->width, video_dec_ctx->height,
                             video_dec_ctx->pix_fmt, 1);
        if (ret < 0) {
            fprintf(stderr, "Could not allocate raw video buffer\n");
            goto end;
        }
        video_dst_bufsize = ret;
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
    if (video_stream)
        printf("Demuxing video from file '%s' into '%s'\n", filename, outfilename);
    
    /* read frames from the file */
    while (av_read_frame(fmt_ctx, &pkt) >= 0) {
        AVPacket orig_pkt = pkt;
        printf("stream index:%u pts:%llu size:%d\n", pkt.stream_index, pkt.pts, pkt.size);
        do {
            ret = decode_packet(&got_frame, 0);
            if (ret < 0)
                break;
            pkt.data += ret;
            pkt.size -= ret;
            //print("loaded %d bytes\n", ret);
        } while (pkt.size > 0);
        av_free_packet(&orig_pkt);
    }
    /* flush cached frames */
    pkt.data = NULL;
    pkt.size = 0;
    do {
        decode_packet(&got_frame, 1);
    } while (got_frame);
    printf("Demuxing succeeded.\n");
    if (video_stream) {
        printf("Play the output video file with the command:\n"
               "ffplay -f rawvideo -pix_fmt %s -video_size %dx%d %s\n",
               av_get_pix_fmt_name(video_dec_ctx->pix_fmt), video_dec_ctx->width, video_dec_ctx->height,
               video_dst_filename);
    }

end:
    return 0;
}
