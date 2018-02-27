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

int main(int argc, char** argv)
{   
    AVFormatContext *fmtCtx = NULL;
    AVIOContext *avioCtx = NULL;
    uint8_t *buf = NULL, *avioCtxBuf = NULL;
    size_t bufSize = 0, avioCtxBufSize = 4096;
    char* inputFileName = NULL;
    int ret = 0;
    struct buffer_data bd = {0};

    inputFileName = "bird.avi";

    // slurp file content into buffer:
    ret = av_file_map(inputFileName, &buf, &bufSize, 0, NULL);
    if (ret < 0) {
        printf("[Error] ret=%d\n", ret);
        return -1;
    }
    printf("end of main()\n");
    return 0;
}
