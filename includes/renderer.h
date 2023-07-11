#ifndef RENDERER_H
#define RENDERER_H

#include "vec3.h"
#include "objects.h"
#include "scene.h"
#include <array>

namespace proxima {
    class Vertex {
    public:
        Vec4 position;
        Vec3 normal;
        Vertex(Vec4 position=Vec4(), Vec3 normal=Vec3(0, 0, 1)) : position(position), normal(normal) {}
    };

    class Face {
    public:
        Vertex *a, *b, *c;
        Face(Vertex *a, Vertex *b, Vertex *c) : a(a), b(b), c(c) {};
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
        Fragment *_fragment_buffer;
        Mat4 _view_matrix;
        Mat4 _projection_matrix;
        void _calc_matrices();
        void _render_object(Object &obj);

    public:
        Renderer(int width, int height);
        ~Renderer();
        int *render(Scene &scene);
    };
}

#endif // RENDERER_H

