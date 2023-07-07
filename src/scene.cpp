#include "scene.h"

namespace proxima {
    Scene::Scene(Vec3 bg_color, float ambient_light) {
        this->bg_color = bg_color;
        this->ambient_light = ambient_light;
    }

    Scene::~Scene() {
        for (auto &entry : this->_objects) {
            delete entry.second;
        }
    }

    Object *&Scene::operator[](std::string obj_name) {
        return this->_objects[obj_name];
    }
}
