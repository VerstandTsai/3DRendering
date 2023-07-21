#include "texture.h"
#include "vec3.h"
#include <vector>

namespace proxima {
    Texture::Texture(int width, int height) {
        this->_width = width;
        this->_height = height;
        this->_data = std::vector<Vec3>(width * height);
    }

    Vec3 Texture::at_uv(Vec3 uv) {
        int x = uv.x * this->_width;
        int y = uv.y * this->_height;
        int index = x + y * this->_width;
        return this->_data[index];
    }

    Texture Texture::Checker(int width, int height) {
        Texture texture(width, height);
        for (int i=0; i<texture._data.size(); i++) {
            texture._data[i] = (i % 2) * Vec3(1, 1, 1);
        }
        return texture;
    }
}

