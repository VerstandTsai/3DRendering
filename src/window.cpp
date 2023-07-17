#include "window.h"
#include <SDL2/SDL.h>

namespace proxima {
    bool Window::_sdl_inited = false;

    Window::Window(int width, int height) {
        this->_width = width;
        this->_height = height;
        this->_mouse_dx = 0;
        this->_mouse_dy = 0;
        if (!_sdl_inited) {
            SDL_Init(SDL_INIT_VIDEO);
            _sdl_inited = true;
        }
        SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_SHOWN, &this->_window, &this->_renderer);
        this->_buffer = SDL_CreateTexture(
            this->_renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            width,
            height
        );
    }

    Window::~Window() {
        SDL_DestroyTexture(this->_buffer);
        SDL_DestroyRenderer(this->_renderer);
        SDL_DestroyWindow(this->_window);
        SDL_Quit();
    }

    bool Window::closed() {
        SDL_Event event;
        bool mouse_moved = false;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                return true;
            case SDL_KEYDOWN:
                this->_keyboard[event.key.keysym.sym] = true;
                break;
            case SDL_KEYUP:
                this->_keyboard[event.key.keysym.sym] = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
                this->_mouse_buttons[event.button.button] = true;
                break;
            case SDL_MOUSEBUTTONUP:
                this->_mouse_buttons[event.button.button] = false;
                break;
            case SDL_MOUSEMOTION:
                mouse_moved = true;
                this->_mouse_dx = event.motion.xrel;
                this->_mouse_dy = event.motion.yrel;
            }
        }
        if (!mouse_moved) {
            this->_mouse_dx = 0;
            this->_mouse_dy = 0;
        }
        return false;
    }

    bool Window::keydown(KeyCode code) {
        if (this->_keyboard.contains(code))
            return this->_keyboard[code];
        return false;
    }

    bool Window::mouse_button_down(MouseButton button) {
        if (this->_mouse_buttons.contains(button))
            return this->_mouse_buttons[button];
        return false;
    }

    void Window::draw(int *buf_rgba) {
        void *pixels;
        int pitch;
        SDL_LockTexture(this->_buffer, NULL, &pixels, &pitch);
        memcpy(pixels, buf_rgba, this->_width * this->_height * sizeof(int));
        SDL_UnlockTexture(this->_buffer);
        SDL_RenderClear(this->_renderer);
        SDL_RenderCopy(this->_renderer, this->_buffer, NULL, NULL);
        SDL_RenderPresent(this->_renderer);
    }
}

