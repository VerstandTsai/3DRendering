#ifndef RENDERER_H
#define RENDERER_H

#include "vec3.h"
#include "objects.h"
#include "buffer.h"
#include <array>

namespace proxima {
    class Renderer {
    private:
        int _width;
        int _height;
        ScreenBuffer _scr_buffer;
        ZBuffer _z_buffer;
        float _d; // The near clipping plane distance
        std::array<Vec3, 2> _get_base_xy();
        Vec3 _project_point(Vec3 p, Vec3 base_x, Vec3 base_y);
        void _scanline(std::array<Vec3, 3> v);

    public:
        Camera camera;
        Renderer(int width, int height);
        ScreenBuffer &render(Object &obj);
    };
}

#endif // RENDERER_H

