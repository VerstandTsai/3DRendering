#pragma once

#include "vec3.h"

#include <vector>

namespace proxima {
    class Texture {
    private:
        int _width;
        int _height;
        std::vector<Vec3> _data;

    public:
        Texture(int width, int height);
        Vec3 at_uv(Vec3 uv);
        static Texture Color(Vec3 color);
        static Texture Checker(int width, int height);
    };
}

