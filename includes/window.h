#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include "buffer.h"

#include <map>

namespace proxima {
    enum KeyCode {
        A = SDLK_a,
        D = SDLK_d,
        S = SDLK_s,
        W = SDLK_w
    };

    class Window {
    private:
        static bool _sdl_inited;
        int _width;
        int _height;
        SDL_Window *_window;
        SDL_Renderer *_renderer;
        std::map<int, bool> _keyboard;

    public:
        Window(int width, int height);
        ~Window();
        bool closed();
        bool keydown(KeyCode code);
        void draw(const ScreenBuffer &buffer);
    };
}

#endif // WINDOW_H

