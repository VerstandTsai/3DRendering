#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include "buffer.h"

namespace proxima {
    class Window {
    private:
        static bool _sdl_inited;
        int _width;
        int _height;
        SDL_Window *_window;
        SDL_Renderer *_renderer;

    public:
        Window(int width, int height);
        ~Window();
        bool closed();
        void draw(const ScreenBuffer &buffer);
    };
}

#endif // WINDOW_H

