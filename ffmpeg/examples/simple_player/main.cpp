extern "C"{
    #include <libavformat/avformat.h> 
    #include <libavcodec/avcodec.h>
    #include <libswresample/swresample.h>
    #include <libswscale/swscale.h>

    #include <libavformat/avformat.h>
    #include <libavformat/avio.h>
    #include <libavutil/opt.h>
    #include <libavutil/file.h>
};

#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"swresample.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"avutil.lib")

#include <stdio.h>

struct buffer_data {
    uint8_t *ptr;
    size_t size; ///< size left in the buffer
};

static int read_packet(void *opaque, uint8_t *buf, int buf_size)
{
    struct buffer_data *bd = (struct buffer_data *)opaque;
    buf_size = FFMIN(buf_size, bd->size);
    if (!buf_size)
        return AVERROR_EOF;
    printf("ptr:%p size:%zu\n", bd->ptr, bd->size);
    /* copy internal buffer data to buf */
    memcpy(buf, bd->ptr, buf_size);
    bd->ptr  += buf_size;
    bd->size -= buf_size;
    return buf_size;
}

int main(int argc, char** argv)
{   
    AVFormatContext* fmtCtx = NULL;
    AVIOContext* avioCtx = NULL;
    uint8_t *buf = NULL, *avioCtxBuf = NULL;
    size_t bufSize = 0, avioCtxBufSize = 4096;
    char* inputFileName = NULL;
    int ret = 0;
    struct buffer_data bd = {0};

    inputFileName = "bird.avi";

    // slurp file content into buffer:
    ret = av_file_map(inputFileName, &buf, &bufSize, 0, NULL);
    if (ret < 0) {
        printf("[Error] line : %d ret=%d\n", __LINE__, ret);
        return -1;
    }

    // fill opaque structure used by the AVIOContext read callback
    bd.ptr = buf;
    bd.size = bufSize;

    if (!(fmtCtx = avformat_alloc_context())) {
        printf("[Error] line : %d ret=%d\n", __LINE__, ret);
        return -1;
    }

    if (!(avioCtxBuf = (uint8_t*)av_malloc(avioCtxBufSize))) {
        printf("[Error] line : %d ret=%d\n", __LINE__, ret);
        return -1;
    }

    avioCtx = avio_alloc_context(avioCtxBuf, 
        avioCtxBufSize, 
        0,
        &bd,
        &read_packet,
        NULL,
        NULL);

    if (!avioCtx) {
        printf("[Error] line : %d ret=%d\n", __LINE__, ret);
        return -1;
    }

    fmtCtx->pb = avioCtx;

    ret = avformat_open_input(&fmtCtx, NULL, NULL, NULL);
    if (ret < 0) {
        printf("[Error] line : %d ret=%d\n", __LINE__, ret);
        return -1;
    }

    ret = avformat_find_stream_info(fmtCtx, NULL);
    if (ret < 0) {
        printf("[Error] line : %d ret=%d\n", __LINE__, ret);
        return -1;
    }

    av_dump_format(fmtCtx, 0, inputFileName, 0);

    avformat_close_input(&fmtCtx);
    if (avioCtx) {
        av_freep(&avioCtx->buffer);
        av_freep(&avioCtx);
    }
    av_file_unmap(buf, bufSize);

    printf("end of main()\n");
    return 0;
}
