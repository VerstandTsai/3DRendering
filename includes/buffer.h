#ifndef BUFFER_H
#define BUFFER_H

#include "vec3.h"
#include <vector>

namespace proxima {
    template<class T>
    class Buffer {
    private:
        int _width;
        int _height;
        T *_pixels;
        int _get_index(int x, int y) const;

    public:
        Buffer(int width, int height);
        ~Buffer();
        T *pixels() const;
        T pixel(int x, int y) const;
        void set_pixel(int x, int y, T p);
        void fill(T p);
    };

    int color2rgba(Vec3 color);
}

#endif // BUFFER_H

