#ifndef EXLOG_NAME
    #undef EXLOG_NAME
    #define EXLOG_NAME "SDLDisplay"
#endif

#include "SDLDisplay.h"
#include <assert.h>

extern "C"
{
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_thread.h>
}

using namespace vh1981lib;

SDLDisplay::SDLDisplay() : _window(nullptr), _texture(nullptr), _renderer(nullptr)
{
    EXCLOG(LOG_INFO, "SDL init...");
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        EXCLOG(LOG_ERROR, "cannot initialize SDL - %s", SDL_GetError());
        assert(false);
    }
}

SDLDisplay::~SDLDisplay()
{
    destroyDisplay();
    SDL_Quit();
}


void SDLDisplay::createDisplay(const int width, const int height)
{
    _window = SDL_CreateWindow("SDL Display", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    if(!_window) {
        EXCLOG(LOG_ERROR, "can't create SDL window - %s", SDL_GetError());
        assert(false);
    }

    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!_renderer){
        SDL_DestroyWindow(_window);
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        assert(false);
    }

    _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, width, height);
    if (!_texture) {
        std::cout << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
        assert(false);
    }
}

void SDLDisplay::destroyDisplay()
{
    if (_texture) {
        SDL_DestroyTexture(_texture);
        _texture = nullptr;
    }
    if (_renderer) {
        SDL_DestroyRenderer(_renderer);
        _renderer = nullptr;
    }
    if (_window) {
        SDL_DestroyWindow(_window);
        _window = nullptr;
    }
}

void SDLDisplay::updateTexture(unsigned char* yPlane, size_t yPitch, \
        unsigned char* uPlane, size_t uPitch, \
        unsigned char* vPlane, size_t vPitch)
{
    SDL_UpdateYUVTexture(_texture, NULL, yPlane, yPitch, uPlane,
            uPitch, vPlane, vPitch);

    SDL_RenderClear(_renderer);
    SDL_RenderCopy(_renderer, _texture, NULL, NULL);
    SDL_RenderPresent(_renderer);
}


