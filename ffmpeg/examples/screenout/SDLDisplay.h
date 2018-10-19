#ifndef SDLDISPLAY_H
#define SDLDISPLAY_H

#include "Display.h"

#include "library/basic/exlog.h"
#include "library/basic/singleton.h"

extern "C"
{
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_thread.h>
}

class SDLDisplay : public singleton<SDLDisplay>
{
public:
    SDLDisplay();
    virtual ~SDLDisplay();

    /**
     @name 초기화
     */
public:
    virtual void createDisplay(const int width, const int height);
    virtual void updateTexture(unsigned char* yPlane, size_t yPitch, \
            unsigned char* uPlane, size_t uPitch, \
            unsigned char* vPlane, size_t vPitch);

private:
    SDL_Window* _window;
    SDL_Renderer* _renderer;
    SDL_Texture* _texture;

};

#endif // SDLDISPLAY_H
