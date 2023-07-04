#include "window.h"
#include <SDL2/SDL.h>

namespace proxima {
    bool Window::_sdl_inited = false;

    Window::Window(int width, int height) {
        this->_width = width;
        this->_height = height;
        if (!_sdl_inited) {
            SDL_Init(SDL_INIT_VIDEO);
            _sdl_inited = true;
        }
        SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_SHOWN, &this->_window, &this->_renderer);
    }

    Window::~Window() {
        SDL_DestroyRenderer(this->_renderer);
        SDL_DestroyWindow(this->_window);
        SDL_Quit();
    }

    bool Window::closed() {
        SDL_Event event;
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
            }
        }
        return false;
    }

    bool Window::keydown(KeyCode code) {
        if (this->_keyboard.contains(code))
            return this->_keyboard[code];
        return false;
    }

    void Window::draw(const ScreenBuffer &buffer) {
        for (int i=0; i<this->_height; i++) {
            for (int j=0; j<this->_width; j++) {
                Vec3 c = buffer.pixel(j, i) * 255;
                SDL_SetRenderDrawColor(this->_renderer, c.x, c.y, c.z, SDL_ALPHA_OPAQUE);
                SDL_RenderDrawPoint(this->_renderer, j, i);
            }
        }
        SDL_RenderPresent(this->_renderer);
    }
}

