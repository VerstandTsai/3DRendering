#include "renderer.h"
#include "objects.h"
#include "scene.h"
#include "vec3.h"
#include <array>
#include <algorithm>
#include <cmath>
#include <vector>

namespace proxima {
    int color2rgba(Vec3 color) {
        int r = fmin(1, color.x) * 255;
        int g = fmin(1, color.y) * 255;
        int b = fmin(1, color.z) * 255;
        return (r << 24) + (g << 16) + (b << 8) + 0xff;
    }

    Vec3 barycentric(Vec3 p, Vec3 a, Vec3 b, Vec3 c) {
        float abc = cross(b - a, c - a).z;
        float pbc = cross(b - p, c - p).z;
        float pca = cross(c - p, a - p).z;
        float pab = cross(a - p, b - p).z;
        return Vec3(pbc, pca, pab) / abc;
    }

    Renderer::Renderer(int width, int height) {
        this->_width = width;
        this->_height = height;
        this->_num_pixels = width * height;
        this->_aspect = (float)width / height;
        this->_frame_buffer = new int[this->_num_pixels];
        this->_depth_buffer = new float[this->_num_pixels];
        this->_fragment_buffer = new Fragment[this->_num_pixels];
    }

    Renderer::~Renderer() {
        delete [] this->_frame_buffer;
        delete [] this->_depth_buffer;
    }

    void Renderer::_calc_matrices() {
        Camera cam = this->_scene->camera;
        float n = cam.near;
        float f = cam.far;
        float s = 1 / tan(deg2rad(cam.fov / 2));
        float x = deg2rad(cam.euler_angles.x);
        float y = deg2rad(cam.euler_angles.y);
        float a = 1 / this->_aspect;
        this->_view_matrix =
              Mat4::RotX(-x)
            * Mat4::RotY(-y)
            * Mat4::Translation(-cam.position);
        this->_projection_matrix = Mat4({{{
            {a*s, 0,        0,          0},
            {  0, s,        0,          0},
            {  0, 0, -f/(f-n), -f*n/(f-n)},
            {  0, 0,       -1,          0}
        }}});
    }

    void Renderer::_render_object(Object &obj) {
        // Calculate all vertices' absolute coordinates
        // and project them onto the screen
        float x = deg2rad(obj.euler_angles.x);
        float y = deg2rad(obj.euler_angles.y);
        float z = deg2rad(obj.euler_angles.z);
        Mat4 model_matrix =
              Mat4::Translation(obj.position)
            * Mat4::RotY(y)
            * Mat4::RotX(x)
            * Mat4::RotZ(z);
        std::vector<Vec3> view_space;
        for (Vec3 vertex : obj.vertices()) {
            Vec3 view_v = this->_view_matrix * model_matrix * vertex;
            view_space.push_back(view_v);
        }

        // Rebuild the vertex-face table
        std::vector<Vertex*> vertices;
        std::vector<Face> faces;
        for (std::array<int, 3> face_index : obj.face_indices()) {
            Vec3 a = view_space[face_index[0]];
            Vec3 b = view_space[face_index[1]];
            Vec3 c = view_space[face_index[2]];
            Vec3 normal = cross(b-a, c-a);
            Vertex *va = new Vertex(a, normal);
            Vertex *vb = new Vertex(b, normal);
            Vertex *vc = new Vertex(c, normal);
            vertices.push_back(va);
            vertices.push_back(vb);
            vertices.push_back(vc);
            faces.push_back(Face(va, vb, vc));
        }

        for (Vertex *v : vertices) {
            v->position = this->_projection_matrix * v->position;
        }

        // Clipping
        faces.erase(std::remove_if(faces.begin(), faces.end(),
            [](Face face) {
                return face.a->position.z < 0 || face.b->position.z < 0 || face.c->position.z < 0;
            }
        ), faces.end());

        auto it = vertices.begin();
        while (it != vertices.end()) {
            if ((*it)->position.z < 0) {
                delete *it;
                it = vertices.erase(it);
            } else it++;
        }

        // Transform to screen space
        for (Vertex *v : vertices) {
            Vec3 ndc_space = v->position;
            int half_width = this->_width >> 1;
            int half_height = this->_height >> 1;
            int screen_x = (ndc_space.x + 1) * half_width;
            int screen_y = (-ndc_space.y + 1) * half_height;
            v->position = Vec3(screen_x, screen_y, ndc_space.z);
            if (screen_x >= 0 && screen_x < this->_width && screen_y >= 0 && screen_y < this->_height) {
                int index = screen_x + screen_y * this->_width;
                this->_frame_buffer[index] = color2rgba(obj.color);
            }
        }

        // Create the fragments
        for (Face face : faces) {
            ;
        }

        // Clean up
        for (Vertex *v : vertices) {
            delete v;
        }
    }

    /*
    Vec3 Renderer::_project_point(Vec3 p) {
        Vec3 view_space = this->_view_matrix * p;
        Vec3 clip_space = this->_projection_matrix * view_space;
        if (clip_space.z < 0 || clip_space.z > 1)
            return Vec3(-1, -1, 2);
        int half_width = this->_width >> 1;
        int half_height = this->_height >> 1;
        int screen_x = (clip_space.x + 1) * half_width;
        int screen_y = (-clip_space.y + 1) * half_height;
        return Vec3(screen_x, screen_y, clip_space.z);
    }
    */

    /*
    Vec3 Renderer::_shade(std::array<Vec3, 3> face, Vec3 color, int shininess) {
        // Calculate the centroid of the face
        Vec3 face_pos = (face[0] + face[1] + face[2]) / 3;

        // Calculate the normal of the face
        Vec3 normal = cross(face[1] - face[0], face[2] - face[0]).normalized();

        // Don't rendering the back of the face
        Vec3 vision = (this->_scene->camera.position - face_pos).normalized();
        if (dot(normal, vision) < 0) return Vec3(-1, -1, -1);

        // Ambient reflection
        Vec3 ambient = this->_scene->ambient_light * Vec3(1, 1, 1);
        Vec3 diffuse = Vec3(0, 0, 0);
        Vec3 specular = Vec3(0, 0, 0);

        for (auto light_source : this->_light_sources) {
            Vec3 face_to_light = light_source->position - face_pos;
            Vec3 light = face_to_light.normalized();
            float distance = face_to_light.magnitude();
            float intensity = ((PointLight*)light_source)->intensity;
            Vec3 light_color = light_source->color * intensity / pow(distance, 2);

            // Diffuse reflection
            float ln = dot(light, normal);
            diffuse += fmax(0, ln) * light_color;

            // Specular reflection
            Vec3 reflection = 2 * ln * normal - light;
            float rv = dot(reflection, vision);
            specular += pow(fmax(0, rv), shininess) * (-1.0 / shininess + 1) * light_color;
        }

        return (ambient + diffuse + specular) * color;
    }
    */

    /*
    void Renderer::_scanline(std::array<Vec3, 3> f, Vec3 color) {
        std::sort(f.begin(), f.end(), [](Vec3 a, Vec3 b) {
            return a.y < b.y;
        }); // Sort the three v of a triangle by their y-value

        for (int y=fmax(0, f[0].y); y<fmin(this->_height, f[2].y); y++) {
            float tac = (y - f[0].y) / (f[2].y - f[0].y);
            Vec3 xzac = lerp(f[0], f[2], tac);
            Vec3 xzb;
            if (y < f[1].y) {
                float tab = (y - f[0].y) / (f[1].y - f[0].y);
                xzb = lerp(f[0], f[1], tab);
            } else {
                float tbc = (y - f[1].y) / (f[2].y - f[1].y);
                xzb = lerp(f[1], f[2], tbc);
            }
            if (xzb.x < xzac.x) std::swap(xzac, xzb);
            int x0 = xzac.x;
            int x1 = xzb.x;
            for (int x=fmax(0, x0); x<fmin(this->_width, x1); x++) {
                float tz = (float)(x - x0) / (x1 - x0);
                float z = lerp(xzac, xzb, tz).z;
                int index = x + y * this->_width;
                if (z < this->_z_buffer[index]) {
                    this->_scr_buffer[index] = color2rgba(color);
                    this->_z_buffer[index] = z;
                }
            }
        }
    }
    */

    int *Renderer::render(Scene &scene) {
        this->_scene = &scene;
        this->_light_sources.clear();
        for (auto &obj_entry : scene.objects()) {
            if (obj_entry.second->is_light())
                this->_light_sources.push_back((PointLight*)(obj_entry.second));
        }
        for (int i=0; i<this->_num_pixels; i++) {
            this->_frame_buffer[i] = color2rgba(scene.bg_color);
            this->_depth_buffer[i] = 1;
        }
        this->_calc_matrices();
        for (auto &obj_entry : this->_scene->objects()) {
            this->_render_object(*obj_entry.second);
        }
        return this->_frame_buffer;
    }
}

