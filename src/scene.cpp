#include "scene.h"

namespace proxima {
    Scene::Scene(Vec3 light_direction) {
        this->light_direction = light_direction;
    }

    Object &Scene::operator[](std::string obj_name) {
        return this->_objects[obj_name];
    }
}
