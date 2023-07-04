#ifndef OBJECTS_H
#define OBJECTS_H

#include "vec3.h"
#include <array>
#include <string>
#include <vector>

namespace proxima {
    class Object {
    protected:
        std::vector<Vec3> _vertices;
        std::vector<std::array<int, 3>> _face_indices;

    public:
        Vec3 position;
        Vec3 euler_angles;
        Vec3 color;
        std::vector<Vec3> &vertices() { return this->_vertices; }
        std::vector<std::array<int, 3>> &face_indices() { return this->_face_indices; }
        Object(Vec3 pos=Vec3(), Vec3 eulers=Vec3(), Vec3 color=Vec3(1, 1, 1));
        Object(std::string filename);
    };

    class Camera : public Object {
    public:
        float fov;
        Camera(float fov=90, Vec3 pos=Vec3(), Vec3 eulers=Vec3());
        Vec3 normal();
    };

    class Cube : public Object {
    private:
        float _width, _height, _depth;

    public:
        Cube(
            float width=1,
            float height=1,
            float depth=1,
            Vec3 color=Vec3(1, 1, 1),
            Vec3 pos=Vec3(),
            Vec3 eulers=Vec3()
        );
        float width() { return this->_width; }
        float height() { return this->_height; }
        float depth() { return this->_depth; }
        void set_size(float width, float height, float depth);
    };

    class Sphere : public Object {
    private:
        float _radius;

    public:
        Sphere(
            float radius=1,
            int resolution=12,
            Vec3 color=Vec3(1, 1, 1),
            Vec3 pos=Vec3(),
            Vec3 eulers=Vec3()
        );
        float radius() { return this->_radius; }
        void set_radius(float radius);
    };
}

#endif // OBJECTS_H

