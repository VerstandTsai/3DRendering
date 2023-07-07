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

            if (obj.is_light()) {
                this->_scanline(proj_v, obj.color);
                continue;
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
        // Calculate the centroid of the face
        Vec3 face_pos = (face[0] + face[1] + face[2]) / 3;

        // Calculate the normal of the face for shading
        Vec3 normal = cross(face[1] - face[0], face[2] - face[0]).normalized();

        // Don't rendering the back of the face
        Vec3 vision = (this->_scene.camera.position - face_pos).normalized();
        if (dot(normal, vision) < 0) return Vec3(-1, -1, -1);

        // Ambient reflection
        Vec3 ambient = this->_scene.ambient_light * color;
        Vec3 diffuse = Vec3(0, 0, 0);
        Vec3 specular = Vec3(0, 0, 0);

        for (auto light_source : this->_light_sources) {
            Vec3 face_to_light = light_source->position - face_pos;
            Vec3 light = face_to_light.normalized();
            float distance = face_to_light.magnitude();
            float luminance = ((PointLight*)light_source)->intensity / pow(distance, 2);

            // Diffuse reflection
            float ln = dot(light, normal);
            diffuse += fmax(0, ln) * color * light_source->color * luminance;

            // Specular reflection
            Vec3 reflection = 2 * ln * normal - light;
            float rv = dot(reflection, vision);
            specular += pow(fmax(0, rv), shininess) * light_source->color * luminance;
        }

        return ambient + diffuse + specular;
    }

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

    int *Renderer::render(Scene &scene) {
        this->_scene = scene;
        this->_light_sources.clear();
        for (auto &obj_entry : scene.objects()) {
            if (obj_entry.second->is_light())
                this->_light_sources.push_back((PointLight*)(obj_entry.second));
        }
        for (int i=0; i<this->_num_pixels; i++) {
            this->_scr_buffer[i] = color2rgba(scene.bg_color);
            this->_z_buffer[i] = INFINITY;
        }
        this->_calc_base_xy();
        for (auto &obj_entry : scene.objects()) {
            this->_render_object(*obj_entry.second);
        }
        return this->_scr_buffer;
    }
}

