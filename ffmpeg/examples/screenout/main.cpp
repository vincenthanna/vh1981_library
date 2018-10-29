#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "Main"
#endif

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

extern "C"
{
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
}


#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"swresample.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"avutil.lib")

#include <iostream>

#include "Player.h"
#include "SDLDisplay.h"
#include "library/basic/exlog.h"


using namespace std;
using namespace vh1981lib;


int main(int argc, char** argv)
{
    EXCLOG(LOG_INFO, "start!\n");
    char *filename, *outfilename;

    SDLDisplay* display = SDLDisplay::get();

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        exit(0);
    }

    filename = argv[1];

    Player player;

    player.play(filename);

    SDLDisplay::get()->destroyDisplay();
    SDL_Quit();

    return 0;
}
