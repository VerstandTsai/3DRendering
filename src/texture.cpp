#include "texture.h"
#include "vec3.h"

#include <string>
#include <vector>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace proxima {
    Texture::Texture(int width, int height) {
        this->_width = width;
        this->_height = height;
        this->_data = std::vector<Vec3>(width * height);
    }

    Texture::Texture(std::string filename) {
        int width, height, depth;
        unsigned char *image = stbi_load(filename.c_str(), &width, &height, &depth, 3);
        this->_width = width;
        this->_height = height;
        for (int i=0; i<width*height*3; i+=3) {
            int r = image[i];
            int g = image[i+1];
            int b = image[i+2];
            this->_data.push_back(Vec3(r, g, b) / 255);
        }
        stbi_image_free(image);
    }

    Vec3 Texture::at_uv(Vec3 uv) const {
        float u = uv.x;
        float v = 1 - uv.y;
        int x = fmin(u * this->_width, this->_width - 1);
        int y = fmin(v * this->_height, this->_height - 1);
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

