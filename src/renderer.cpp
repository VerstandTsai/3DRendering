#include "renderer.h"
#include "buffer.h"
#include "objects.h"
#include "vec3.h"
#include <array>
#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

namespace proxima {
    Renderer::Renderer(int width, int height) :
        _width(width),
        _height(height),
        _scr_buffer(width, height),
        _z_buffer(width, height),
        _d(0.1) {}

    void Renderer::_calc_base_xy() {
        // Find the normalized x and y vector on the plane
        // Find x first because it is always perpendicular to the world y-axis
        Vec3 camera_x = cross(this->_camera.normal(), Vec3(0, 1, 0)).normalized();
        Vec3 camera_y = cross(camera_x, this->_camera.normal()).normalized();

        // Nomarlize x and y proportioanl to the size of the plane
        float plane_half_width = this->_d * tan(this->_camera.fov / 2);
        this->_base_x = plane_half_width / (this->_width / 2.0) * camera_x;
        this->_base_y = _base_x.magnitude() * camera_y;
    }

    void Renderer::_render_object(Object &obj) {
        // Project all vertices onto the screen
        std::vector<Vec3> adjusted_vertices;
        std::vector<Vec3> projected_vertices;
        for (Vec3 vertex : obj.vertices()) {
            Vec3 adjusted = rotate(vertex, obj.euler_angles) + obj.position;
            adjusted_vertices.push_back(adjusted);
            projected_vertices.push_back(this->_project_point(adjusted));
        }

        // Scanline each face with face-vertex indices table
        for (std::array<int, 3> face_index : obj.face_indices()) {
            std::array<Vec3, 3> adj_v;
            std::array<Vec3, 3> proj_v;
            for (int i=0; i<3; i++) {
                adj_v[i] = adjusted_vertices[face_index[i]];
                proj_v[i] = projected_vertices[face_index[i]];
            }
            Vec3 face_normal = cross(adj_v[1] - adj_v[0], adj_v[2] - adj_v[0]).normalized();
            float prod = dot(face_normal, this->_light_direction);
            Vec3 color;
            color = std::abs(prod) * Vec3(1, 1, 1) * (prod < 0) + (1 - std::abs(prod)) * obj.color;
            this->_scanline(proj_v, color);
        }
    }

    Vec3 Renderer::_project_point(Vec3 p) {
        // Project p onto the viewing plane
        // and let the center of the plane be the origin
        Vec3 op = p - this->_camera.position;
        Vec3 p_on_plane = this->_d * op / dot(this->_camera.normal(), op);
        Vec3 o_on_plane = this->_d * this->_camera.normal();
        Vec3 op_on_plane = p_on_plane - o_on_plane;

        // Find the 2d coordinates of op on the plane with x and y base vectors
        float x_coor = dot(op_on_plane, this->_base_x) / this->_base_x.magnitude() / this->_base_x.magnitude();
        float y_coor = dot(op_on_plane, this->_base_y) / this->_base_y.magnitude() / this->_base_y.magnitude();
        x_coor += this->_width / 2.0;
        y_coor = -y_coor + this->_height / 2.0;

        return Vec3(round(x_coor), round(y_coor), op.magnitude());
    }

    void Renderer::_scanline(std::array<Vec3, 3> v, Vec3 color) {
        std::sort(v.begin(), v.end(), [](Vec3 a, Vec3 b) {
            return a.y < b.y;
        }); // Sort the three v of a triangle by their y-value

        // Calculate the inverse slope of the three sides
        Vec3 dab = (v[1] - v[0]) / (v[1].y - v[0].y);
        Vec3 dac = (v[2] - v[0]) / (v[2].y - v[0].y);
        Vec3 dbc = (v[2] - v[1]) / (v[2].y - v[1].y);

        bool flat_top = v[1].y == v[0].y;

        Vec3 vs[] = {flat_top ? v[1] : v[0], v[0]};
        for (int y=v[0].y; y<v[2].y; y++) {
            int x0 = round(vs[0].x);
            int x1 = round(vs[1].x);
            float z = vs[0].z;
            float dz = (vs[1].z - vs[0].z) / (x1 - x0);
            if (x0 > x1) {
                std::swap(x0, x1);
                z = vs[1].z;
            }
            for (int x=x0; x<x1; x++) {
                if (
                    x >= 0 && x < this->_width &&
                    y >= 0 && y < this->_height &&
                    z < this->_z_buffer.pixel(x, y)
                ) {
                    this->_scr_buffer.set_pixel(x, y, color);
                    this->_z_buffer.set_pixel(x, y, z);
                }
                z += dz;
            }
            vs[0] += y < v[1].y ? dab : dbc;
            vs[1] += dac;
        }
    }

    ScreenBuffer &Renderer::render(Scene &scene) {
        this->_scr_buffer.fill(Vec3(0, 0, 0));
        this->_z_buffer.fill(std::numeric_limits<float>::infinity());
        this->_camera = scene.camera;
        this->_light_direction = scene.light_direction;
        this->_calc_base_xy();
        for (auto &obj_entry : scene.objects()) {
            this->_render_object(obj_entry.second);
        }
        return this->_scr_buffer;
    }
}

