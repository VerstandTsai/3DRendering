#include "buffer.h"
#include "vec3.h"

#include <vector>
#include <array>

namespace proxima {
    template<class T>
    Buffer<T>::Buffer(int width, int height) {
        this->_width = width;
        this->_height = height;
        this->_pixels = new T[width * height];
        for (int i=0; i<width*height; i++) {
            this->_pixels[i] = T();
        }
    }

    template<class T>
    Buffer<T>::~Buffer() {
        delete [] this->_pixels;
    }

    template<class T>
    int Buffer<T>::_get_index(int x, int y) const {
        return y * this->_width + x;
    }

    template<class T>
    T *Buffer<T>::pixels() const {
        return this->_pixels;
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
        for (int i=0; i<this->_width*this->_height; i++)
            this->_pixels[i] = p;
    }

    template class Buffer<int>;
    template class Buffer<float>;

    int color2rgba(Vec3 color) {
        Vec3 c = color * 255;
        return ((int)c.x << 24) + ((int)c.y << 16) + ((int)c.z << 8) + 0xff;
    }
}

