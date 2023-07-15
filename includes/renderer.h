#ifndef RENDERER_H
#define RENDERER_H

#include "vec3.h"
#include "objects.h"
#include "scene.h"
#include <array>
#include <vector>

namespace proxima {
    class Vertex {
    public:
        Vec4 position;
        Vec3 normal;
        Vec3 view_pos;
        Vertex(Vec4 position=Vec4(), Vec3 normal=Vec3(0, 0, 1), Vec3 view_pos=Vec3()) :
            position(position), normal(normal), view_pos(view_pos) {}
    };

    class Face {
    public:
        std::array<Vertex*, 3> vertices;
        Face(std::array<Vertex*, 3> vertices) : vertices(vertices) {};
    };

    class Fragment {
    public:
        Vec3 color;
        Vec3 normal;
        Vec3 view_pos;
        float depth;
        Vec3 vision;
        bool is_light;
        float shininess;
        Fragment(Vec3 color=Vec3(), Vec3 normal=Vec3(0, 0, -1), Vec3 view_pos=Vec3(), float depth=1) :
            color(color), normal(normal), view_pos(view_pos), depth(depth) {}
    };

    class Renderer {
    private:
        int _width;
        int _height;
        int _num_pixels;
        float _aspect;
        Scene *_scene;
        std::vector<PointLight> _light_sources;
        int *_frame_buffer;
        Fragment *_fragment_buffer;
        Mat4 _view_rotation;
        Mat4 _view_matrix;
        Mat4 _projection_matrix;
        void _calc_matrices();
        void _calc_cam_normals();
        void _rasterize(Face face, Vec3 color, bool is_light, float shininess);
        void _shade(int index);
        void _render_object(Object &obj);

    public:
        Renderer(int width, int height);
        ~Renderer();
        int *render(Scene &scene);
    };
}

#endif // RENDERER_H

