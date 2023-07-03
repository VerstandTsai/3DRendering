#ifndef SCENE_H
#define SCENE_H

#include "objects.h"

#include <string>
#include <map>

namespace proxima {
    class Scene {
    private:
        std::map<std::string, Object> _objects;

    public:
    };
}

#endif // SCENE_H

