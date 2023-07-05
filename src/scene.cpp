#include "scene.h"

namespace proxima {
    Scene::Scene(Vec3 bg_color, Vec3 light_direction, float ambient_light) {
        this->bg_color = bg_color;
        this->light_direction = light_direction;
        this->ambient_light = ambient_light;
    }

    Object &Scene::operator[](std::string obj_name) {
        return this->_objects[obj_name];
    }
}
