extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
};

#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"swresample.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"avutil.lib")

class A {
public:
    A(){}
    int tempValue;
    int a;
    int b;
};

int main(int argc,char** argv)
{
    av_register_all();
    avcodec_register_all();
    return 0;
}