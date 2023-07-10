#ifndef RENDERER_H
#define RENDERER_H

#include "vec3.h"
#include "objects.h"
#include "scene.h"
#include <array>

namespace proxima {
    class Vertex {
    public:
        Vec3 position;
        Vec3 color;
        Vec3 normal;
        Vertex(Vec3 position=Vec3(0, 0, 0), Vec3 color=Vec3(1, 1, 1), Vec3 normal=Vec3(0, 0, 1)) :
            position(position), color(color), normal(normal) {}
    };

    class Fragment {
    public:
        Vec3 color;
        Vec3 normal;
        Fragment(Vec3 color=Vec3(1, 1, 1), Vec3 normal=Vec3(0, 0, 1)) :
            color(color), normal(normal) {}
    };

    class Renderer {
    private:
        int _width;
        int _height;
        int _num_pixels;
        float _aspect;
        Scene *_scene;
        std::vector<PointLight*> _light_sources;
        int *_frame_buffer;
        float *_depth_buffer;
        Fragment *fragment_buffer;
        std::vector<Vertex> vertices;
        Mat4 _view_matrix;
        Mat4 _projection_matrix;
        void _calc_matrices();

    public:
        Renderer(int width, int height);
        ~Renderer();
        int *render(Scene &scene);
    };
}

#endif // RENDERER_H

