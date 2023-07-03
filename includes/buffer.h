#ifndef BUFFER_H
#define BUFFER_H

#include <vector>

namespace proxima {
    class Color {
    public:
        unsigned char r, g, b;
        Color() : r(0), g(0), b(0) {};
        Color(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {};
    };

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

    typedef Buffer<Color> ScreenBuffer;
    typedef Buffer<float> ZBuffer;
}

#endif // BUFFER_H

