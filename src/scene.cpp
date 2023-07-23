#include "scene.h"
#include "objects.h"
#include "texture.h"

namespace proxima {
    Scene::Scene(Texture skybox, float ambient_light) {
        this->skybox = skybox;
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

