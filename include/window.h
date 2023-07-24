#pragma once

#include <SDL2/SDL.h>
#include <map>

namespace proxima {
    enum KeyCode {
        A = SDLK_a,
        D = SDLK_d,
        S = SDLK_s,
        W = SDLK_w,
        SPACE = SDLK_SPACE,
        LSHIFT = SDLK_LSHIFT
    };

    enum MouseButton {
        LEFT = SDL_BUTTON_LEFT,
        MIDDLE = SDL_BUTTON_MIDDLE,
        RIGHT = SDL_BUTTON_RIGHT
    };

    class Window {
    private:
        static bool _sdl_inited;
        int _width;
        int _height;
        SDL_Window *_window;
        SDL_Renderer *_renderer;
        SDL_Texture *_buffer;
        std::map<int, bool> _keyboard;
        std::map<int, bool> _mouse_buttons;
        int _mouse_dx;
        int _mouse_dy;

    public:
        Window(int width, int height);
        ~Window();
        bool closed();
        bool keydown(KeyCode code);
        bool mouse_button_down(MouseButton button);
        int mouse_dx() { return this->_mouse_dx; }
        int mouse_dy() { return this->_mouse_dy; }
        void draw(int *buf_rgba);
    };
}

