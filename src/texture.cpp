#include "texture.h"
#include "vec3.h"
#include <vector>
#include <cmath>

namespace proxima {
    Texture::Texture(int width, int height) {
        this->_width = width;
        this->_height = height;
        this->_data = std::vector<Vec3>(width * height);
    }

    Vec3 Texture::at_uv(Vec3 uv) const {
        int x = fmin(uv.x * this->_width, this->_width - 1);
        int y = fmin(uv.y * this->_height, this->_height - 1);
        int index = x + y * this->_width;
        return this->_data[index];
    }

    Texture Texture::Color(Vec3 color) {
        Texture texture(1, 1);
        texture._data[0] = color;
        return texture;
    }

    Texture Texture::Checker(int width, int height) {
        Texture texture(width, height);
        int index = 0;
        for (int i=0; i<height; i++) {
            for (int j=0; j<width; j++) {
                texture._data[index] = ((i + j) % 2) * Vec3(1, 1, 1);
                index++;
            }
        }
        return texture;
    }
}

