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

    std::array<Vec3, 2> Renderer::_get_base_xy() {
        // Find the normalized x and y vector on the plane
        // Find x first because it is always perpendicular to the world y-axis
        Vec3 camera_x = cross(this->camera.normal(), Vec3(0, 1, 0));
        camera_x /= camera_x.norm();
        Vec3 camera_y = cross(camera_x, this->camera.normal());
        camera_y /= camera_y.norm();

        // Nomarlize x and y proportioanl to the size of the plane
        float plane_half_width = this->_d * tan(this->camera.fov / 2);
        Vec3 base_x = plane_half_width / (this->_width / 2.0) * camera_x;
        Vec3 base_y = base_x.norm() * camera_y;

        return {base_x, base_y};
    }

    Vec3 Renderer::_project_point(Vec3 p, Vec3 base_x, Vec3 base_y) {
        // Project p onto the viewing plane
        // and let the center of the plane be the origin
        Vec3 op = p - this->camera.position;
        Vec3 p_on_plane = this->_d * op / dot(this->camera.normal(), op);
        Vec3 o_on_plane = this->_d * this->camera.normal();
        Vec3 op_on_plane = p_on_plane - o_on_plane;

        // Find the 2d coordinates of op on the plane with x and y base vectors
        float x_coor = dot(op_on_plane, base_x) / base_x.norm() / base_x.norm();
        float y_coor = dot(op_on_plane, base_y) / base_y.norm() / base_y.norm();
        x_coor += this->_width / 2.0;
        y_coor = -y_coor + this->_height / 2.0;

        return Vec3(round(x_coor), round(y_coor), op.norm());
    }

    void Renderer::_scanline(std::array<Vec3, 3> v) {
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
                    this->_scr_buffer.set_pixel(x, y, Vec3(1, 1, 1));
                    this->_z_buffer.set_pixel(x, y, z);
                }
                z += dz;
            }
            vs[0] += y < v[1].y ? dab : dbc;
            vs[1] += dac;
        }
    }

    ScreenBuffer &Renderer::render(Object &obj) {
        this->_scr_buffer.fill(Vec3(0, 0, 0));
        this->_z_buffer.fill(std::numeric_limits<float>::infinity());

        // Project all vertices onto the screen
        std::array<Vec3, 2> base_xy = this->_get_base_xy();
        std::vector<Vec3> projected_vertices;
        for (Vec3 vertex : obj.vertices()) {
            Vec3 adjusted = rotate(vertex, obj.euler_angles) + obj.position;
            projected_vertices.push_back(this->_project_point(adjusted, base_xy[0], base_xy[1]));
        }

        // Scanline each face with face-vertex indices table
        for (std::array<int, 3> face_index : obj.face_indices()) {
            std::array<Vec3, 3> v;
            for (int i=0; i<3; i++) {
                v[i] = projected_vertices[face_index[i]];
            }
            this->_scanline(v);
        }
        return this->_scr_buffer;
    }
}

