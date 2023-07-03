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
        std::vector<T> _pixels;
        int _get_index(int x, int y) const;

    public:
        Buffer(int width, int height);
        T pixel(int x, int y) const;
        void set_pixel(int x, int y, T p);
        void fill(T p);
    };

    typedef Buffer<Vec3> ScreenBuffer;
    typedef Buffer<float> ZBuffer;
}

#endif // BUFFER_H

