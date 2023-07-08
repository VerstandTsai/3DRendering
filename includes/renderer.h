#ifndef RENDERER_H
#define RENDERER_H

#include "vec3.h"
#include "objects.h"
#include "scene.h"
#include <array>

namespace proxima {
    class Renderer {
    private:
        int _width;
        int _height;
        int _num_pixels;
        float _aspect;
        int *_scr_buffer;
        float *_z_buffer;
        Scene *_scene;
        std::vector<PointLight*> _light_sources;
        Mat4 _view_matrix;
        Mat4 _projection_matrix;
        void _calc_matrices();
        void _render_object(Object &obj);
        Vec3 _project_point(Vec3 p);
        Vec3 _shade(std::array<Vec3, 3> face, Vec3 color, int shininess); 
        void _scanline(std::array<Vec3, 3> f, Vec3 color);

    public:
        Renderer(int width, int height);
        ~Renderer();
        int *render(Scene &scene);
    };
}

#endif // RENDERER_H

