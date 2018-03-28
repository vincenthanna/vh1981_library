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

#include "Player.h"
#include "MotionSearch.h"


using namespace std;
using namespace VideoAnalytics;

int main(int argc, char** argv)
{
    char *filename, *outfilename;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        exit(0);
    }

    filename = argv[1];

    Player player;

    MotionOptions motionOptions;
    motionOptions.minBlocks = 4;
    motionOptions.motionSensitivity = 4;

    player.play(filename, motionOptions);

    return 0;
}

