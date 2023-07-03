#ifndef SCENE_H
#define SCENE_H

#include "objects.h"
#include "vec3.h"

#include <string>
#include <map>

namespace proxima {
    class Scene {
    private:
        std::map<std::string, Object> _objects;

    public:
        Camera camera;
        Vec3 light_direction;
        Scene(Vec3 light_direction=Vec3(0, -1, 0));
        std::map<std::string, Object> objects() { return this->_objects; }
        Object &operator[](std::string obj_name);
    };
}

#endif // SCENE_H

