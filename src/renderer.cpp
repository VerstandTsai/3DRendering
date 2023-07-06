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
        Vec3 c = color * 255;
        return ((int)c.x << 24) + ((int)c.y << 16) + ((int)c.z << 8) + 0xff;
    }

    Renderer::Renderer(int width, int height) : _scene(this->_dummy) {
        this->_width = width;
        this->_height = height;
        this->_num_pixels = width * height;
        this->_d = 0.1;
        this->_scr_buffer = new int[this->_num_pixels];
        this->_z_buffer = new float[this->_num_pixels];
    }

    Renderer::~Renderer() {
        delete [] this->_scr_buffer;
        delete [] this->_z_buffer;
    }

    void Renderer::_calc_base_xy() {
        // Find the normalized x and y vector on the plane
        // Find x first because it is always perpendicular to the world y-axis
        Vec3 camera_x = cross(this->_scene.camera.normal(), Vec3(0, 1, 0)).normalized();
        Vec3 camera_y = cross(camera_x, this->_scene.camera.normal()).normalized();

        // Nomarlize x and y proportioanl to the size of the plane
        float plane_half_width = this->_d * tan(this->_scene.camera.fov / 2);
        this->_base_x = plane_half_width / (this->_width / 2.0) * camera_x;
        this->_base_y = _base_x.magnitude() * camera_y;
    }

    void Renderer::_render_object(Object &obj) {
        // Calculate all vertices' absolute coordinates
        // and project them onto the screen
        std::vector<Vec3> absolute_vertices;
        std::vector<Vec3> projected_vertices;
        for (Vec3 vertex : obj.vertices()) {
            Vec3 absolute = rotate(vertex, obj.euler_angles) + obj.position;
            absolute_vertices.push_back(absolute);
            projected_vertices.push_back(this->_project_point(absolute));
        }

        // Scanline each face using the face-vertex indices table
        for (std::array<int, 3> face_index : obj.face_indices()) {
            std::array<Vec3, 3> abs_v;
            std::array<Vec3, 3> proj_v;
            for (int i=0; i<3; i++) {
                abs_v[i] = absolute_vertices[face_index[i]];
                proj_v[i] = projected_vertices[face_index[i]];
            }

            Vec3 color = this->_shade(abs_v, obj.color, obj.shininess);
            if (color.x < 0) continue;
            this->_scanline(proj_v, color);
        }
    }

    Vec3 Renderer::_project_point(Vec3 p) {
        // Project p onto the viewing plane
        // and let the center of the plane be the origin
        Vec3 op = p - this->_scene.camera.position;
        if (dot(op, this->_scene.camera.normal()) / this->_scene.camera.normal().magnitude() < this->_d)
            return Vec3(-1, -1, INFINITY);
        Vec3 p_on_plane = this->_d * op / dot(this->_scene.camera.normal(), op);
        Vec3 o_on_plane = this->_d * this->_scene.camera.normal();
        Vec3 op_on_plane = p_on_plane - o_on_plane;

        // Find the 2d coordinates of op on the plane with x and y base vectors
        float x_coor = dot(op_on_plane, this->_base_x) / pow(this->_base_x.magnitude(), 2);
        float y_coor = dot(op_on_plane, this->_base_y) / pow(this->_base_y.magnitude(), 2);
        x_coor += this->_width / 2.0;
        y_coor = -y_coor + this->_height / 2.0;

        return Vec3(round(x_coor), round(y_coor), op.magnitude());
    }

    Vec3 Renderer::_shade(std::array<Vec3, 3> face, Vec3 color, int shininess) {
        // Calculate the normal of the face for shading
        Vec3 face_normal = cross(face[1] - face[0], face[2] - face[0]).normalized();

        // Don't rendering the back of the face
        Vec3 op = (face[0] - this->_scene.camera.position).normalized();
        if (dot(face_normal, op) > 0) return Vec3(-1, -1, -1);

        // Ambient shading
        Vec3 draw_color = this->_scene.ambient_light * color;

        // Diffuse shading
        Vec3 light = -this->_scene.light_direction;
        float nl = dot(face_normal, light);
        float brightness = fmax(0, nl);
        draw_color = lerp(draw_color, color, brightness);

        // Specular shading
        Vec3 reflection = 2 * nl * face_normal - light;
        float prod = dot(-this->_scene.camera.normal(), reflection);
        float luminance = pow(fmax(0, prod), shininess);
        draw_color = lerp(draw_color, Vec3(1, 1, 1), luminance);

        return draw_color;
    }

    void Renderer::_scanline(std::array<Vec3, 3> f, Vec3 color) {
        std::sort(f.begin(), f.end(), [](Vec3 a, Vec3 b) {
            return a.y < b.y;
        }); // Sort the three v of a triangle by their y-value

        // Calculate the inverse slope of the three sides
        Vec3 dab = (f[1] - f[0]) / (f[1].y - f[0].y);
        Vec3 dac = (f[2] - f[0]) / (f[2].y - f[0].y);
        Vec3 dbc = (f[2] - f[1]) / (f[2].y - f[1].y);

        bool flat_top = f[1].y == f[0].y;

        Vec3 vs[] = {flat_top ? f[1] : f[0], f[0]};
        for (int y=f[0].y; y<f[2].y; y++) {
            int x0 = round(vs[0].x);
            int x1 = round(vs[1].x);
            float z = vs[0].z;
            float dz = (vs[1].z - vs[0].z) / (x1 - x0);
            if (x0 > x1) {
                std::swap(x0, x1);
                z = vs[1].z;
            }
            for (int x=x0; x<x1; x++) {
                int index = x + y * this->_width;
                if (
                    x >= 0 && x < this->_width &&
                    y >= 0 && y < this->_height &&
                    z < this->_z_buffer[index]
                ) {
                    this->_scr_buffer[index] = color2rgba(color);
                    this->_z_buffer[index] = z;
                }
                z += dz;
            }
            vs[0] += y < f[1].y ? dab : dbc;
            vs[1] += dac;
        }
    }

    int *Renderer::render(Scene &scene) {
        for (int i=0; i<this->_num_pixels; i++) {
            this->_scr_buffer[i] = color2rgba(scene.bg_color);
            this->_z_buffer[i] = INFINITY;
        }
        this->_scene = scene;
        this->_calc_base_xy();
        for (auto &obj_entry : scene.objects()) {
            this->_render_object(obj_entry.second);
        }
        return this->_scr_buffer;
    }
}

