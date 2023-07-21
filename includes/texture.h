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
        Texture(int width=1, int height=1);
        Vec3 at_uv(Vec3 uv) const;
        static Texture Color(Vec3 color);
        static Texture Checker(int width, int height);
    };
}

