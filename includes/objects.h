#ifndef OBJECTS_H
#define OBJECTS_H

#include "vec3.h"
#include <array>
#include <vector>

namespace proxima {
    class Object {
    protected:
        std::vector<Vec3> _vertices;
        std::vector<std::array<int, 3>> _face_indices;

    public:
        Vec3 position;
        Vec3 euler_angles;
        std::vector<Vec3> &vertices() { return this->_vertices; }
        std::vector<std::array<int, 3>> &face_indices() { return this->_face_indices; }
        Object(Vec3 pos);
    };

    class Camera : public Object {
    public:
        float fov;
        Camera(Vec3 pos, float fov) : Object(pos) { this->fov = fov; }
        Vec3 normal();
    };

    class Cube : public Object {
    private:
        float _width, _height, _depth;

    public:
        Cube(Vec3 pos) : Cube(pos, 1, 1, 1) {}
        Cube(Vec3 pos, float width, float height, float depth);
        float width() { return this->_width; }
        float height() { return this->_height; }
        float depth() { return this->_depth; }
        void set_size(float width, float height, float depth);
    };
}

#endif // OBJECTS_H

