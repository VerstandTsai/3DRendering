#include "buffer.h"

#include <vector>
#include <array>

namespace proxima {
    template<class T>
    Buffer<T>::Buffer(int width, int height) {
        this->_width = width;
        this->_height = height;
        for (int i=0; i<width*height; i++) {
            this->_pixels.push_back(T());
        }
    }

    template<class T>
    int Buffer<T>::_get_index(int x, int y) const {
        return y * this->_width + x;
    }

    template<class T>
    T Buffer<T>::pixel(int x, int y) const {
        return this->_pixels[this->_get_index(x, y)];
    }

    template<class T>
    void Buffer<T>::set_pixel(int x, int y, T p) {
        this->_pixels[this->_get_index(x, y)] = p;
    }

    template<class T>
    void Buffer<T>::fill(T p) {
        this->_pixels = std::vector<T>(this->_width * this->_height, p);
    }

    template class Buffer<Color>;
    template class Buffer<float>;
}

