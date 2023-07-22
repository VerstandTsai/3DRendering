#pragma once

#include "vec3.h"

#include <vector>
#include <string>

namespace proxima {
    class Texture {
    private:
        int _width;
        int _height;
        std::vector<Vec3> _data;

    public:
        Texture(int width=1, int height=1);
        Texture(std::string filename);
        inline Vec3 at_uv(Vec3 uv) const;
        static Texture Color(Vec3 color);
        static Texture Checker(int width, int height);
    };

    Vec3 Texture::at_uv(Vec3 uv) const {
        float u = uv.x;
        float v = 1 - uv.y;
        int x = fmin(u * this->_width, this->_width - 1);
        int y = fmin(v * this->_height, this->_height - 1);
        int index = x + y * this->_width;
        return this->_data[index];
    }
}

