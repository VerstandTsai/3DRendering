#pragma once

#include "vec3.h"
#include <array>
#include <vector>
#include <string>

namespace proxima {
    class Mesh {
    private:
        std::vector<Vec3> _vertices;
        std::vector<Vec3> _vertex_normals;
        std::vector<Vec3> _uv_coordinates;
        std::vector<std::array<int, 9>> _face_indices;
        bool _has_normal;
        bool _has_uv;

    public:
        bool has_normal() const { return this->_has_normal; }
        bool has_uv() const { return this->_has_uv; }
        const std::vector<Vec3> &vertices() const { return this->_vertices; }
        const std::vector<Vec3> &vertex_normals() const { return this->_vertex_normals; }
        const std::vector<Vec3> &uv_coordinates() const { return this->_uv_coordinates; }
        const std::vector<std::array<int, 9>> &face_indices() const { return this->_face_indices; }
        Mesh() : _has_normal(false), _has_uv(false) {};
        Mesh(std::string filename);
        static Mesh Cube();
        static Mesh Skybox();
        static Mesh Sphere(int resolution=20);
        static Mesh Torus(float thickness=0.5, int resolution=20);
    };
}

