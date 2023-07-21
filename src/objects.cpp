#include "objects.h"
#include "mesh.h"
#include "texture.h"
#include "vec3.h"

#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>

namespace proxima {
    Object::Object(Mesh mesh, Texture texture, int shininess) {
        this->_mesh = mesh;
        this->texture = texture;
        this->shininess = shininess;
        this->_is_light = false;
        this->position = Vec3();
        this->euler_angles = Vec3();
        this->scale = Vec3(1, 1, 1);
    }

    Camera::Camera(float fov, float near, float far) : Object() {
        this->fov = fov;
        this->near = near;
        this->far = far;
    }

    PointLight::PointLight(float intensity, Vec3 color) : Object(Mesh::Cube(), Texture::Color(color)) {
        this->_is_light = true;
        this->color = color;
        this->intensity = intensity;
        this->scale = Vec3(0.1, 0.1, 0.1);
    }
}

