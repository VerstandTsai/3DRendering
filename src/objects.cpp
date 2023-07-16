#include "objects.h"
#include "mesh.h"
#include "vec3.h"

#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>

namespace proxima {
    Object::Object(Mesh mesh, Vec3 color, int shininess) {
        this->_mesh = mesh;
        this->color = color;
        this->shininess = shininess;
        this->_is_light = false;
        this->position = Vec3();
        this->euler_angles = Vec3();
    }

    Camera::Camera(float fov, float near, float far) : Object() {
        this->fov = fov;
        this->near = near;
        this->far = far;
    }

    PointLight::PointLight(float intensity, Vec3 color) : Object(Mesh::Cube(), color) {
        this->_is_light = true;
        this->intensity = intensity;
    }
}

