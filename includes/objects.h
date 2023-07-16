#pragma once

#include "vec3.h"
#include "mesh.h"
#include <array>
#include <vector>

namespace proxima {
    class Object {
    protected:
        Mesh _mesh;
        bool _is_light;

    public:
        Vec3 position;
        Vec3 euler_angles;
        Vec3 color;
        int shininess;
        const Mesh &mesh() const { return this->_mesh; }
        bool is_light() const { return this->_is_light; }
        Object(
            Mesh mesh=Mesh::Cube(),
            Vec3 color=Vec3(1, 1, 1),
            int shininess=32
        );
    };

    class Camera : public Object {
    public:
        float fov;
        float near;
        float far;
        Camera(float fov=90, float near=0.1, float far=1000);
    };

    class PointLight : public Object {
    public:
        float intensity;
        PointLight(float intensity=20, Vec3 color=Vec3(1, 1, 1));
    };
}

