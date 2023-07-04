#include "scene.h"

namespace proxima {
    Scene::Scene(Vec3 bg_color, Vec3 light_direction) {
        this->bg_color = bg_color;
        this->light_direction = light_direction;
    }

    Object &Scene::operator[](std::string obj_name) {
        return this->_objects[obj_name];
    }
}
