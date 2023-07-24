#pragma once

#include "objects.h"
#include "texture.h"
#include "vec3.h"
#include <string>
#include <map>

namespace proxima {
    class Scene {
    private:
        std::map<std::string, Object*> _objects;

    public:
        Camera camera;
        Texture skybox;
        float ambient_light;
        Scene(Texture skybox=Texture::Color(Vec3(0, 0, 0)), float ambient_light=0.1);
        ~Scene();
        const std::map<std::string, Object*> &objects() const { return this->_objects; }
        Object *&operator[](std::string obj_name);
    };
}

