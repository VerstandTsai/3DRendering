#include "texture.h"
#include "vec3.h"

#include <string>
#include <vector>
#include <cmath>

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
                int t = (i + j) & 1;
                texture._data[index] = lerp(Vec3(0.1, 0.1, 0.1), Vec3(1, 1, 1), t);
                index++;
            }
        }
        return texture;
    }
}

