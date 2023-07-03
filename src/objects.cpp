#include "objects.h"
#include "vec3.h"

namespace proxima {
    Object::Object(Vec3 pos, Vec3 eulers, Vec3 color) {
        this->position = pos;
        this->euler_angles = eulers;
        this->color = color;
    }

    Camera::Camera(Vec3 pos, Vec3 eulers, float fov) : Object(pos, eulers) {
        this->fov = fov;
    }

    Vec3 Camera::normal() {
        return rotate(Vec3(0, 0, -1), this->euler_angles);
    }

    Cube::Cube(Vec3 pos, Vec3 eulers, Vec3 color, float width, float height, float depth)
        : Object(pos, eulers, color) 
    {
        this->_width = width;
        this->_height = height;
        this->_depth = depth;
        for (int i=0; i<2; i++) {
            for (int j=0; j<2; j++) {
                for (int k=0; k<2; k++) {
                    float x = (i - 0.5) * width;
                    float y = (j - 0.5) * height;
                    float z = (k - 0.5) * depth;
                    this->_vertices.push_back(Vec3(x, y, z));
                }
            }
        }
        this->_face_indices.push_back({0, 1, 3});
        this->_face_indices.push_back({3, 2, 0});
        this->_face_indices.push_back({0, 2, 6});
        this->_face_indices.push_back({6, 4, 0});
        this->_face_indices.push_back({0, 4, 5});
        this->_face_indices.push_back({5, 1, 0});
        this->_face_indices.push_back({1, 5, 7});
        this->_face_indices.push_back({7, 3, 1});
        this->_face_indices.push_back({2, 3, 7});
        this->_face_indices.push_back({7, 6, 2});
        this->_face_indices.push_back({5, 4, 6});
        this->_face_indices.push_back({6, 7, 5});
    }
}

