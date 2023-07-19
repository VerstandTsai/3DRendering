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
        std::vector<std::array<int, 6>> _face_indices;
        bool _has_normal;

    public:
        bool has_normal() const { return this->_has_normal; }
        const std::vector<Vec3> &vertices() const { return this->_vertices; }
        const std::vector<Vec3> &vertex_normals() const { return this->_vertex_normals; }
        const std::vector<std::array<int, 6>> &face_indices() const { return this->_face_indices; }
        Mesh() { this->_has_normal = false; };
        Mesh(std::string filename);
        static Mesh Cube();
        static Mesh Sphere(int resolution=20);
        static Mesh Torus(float thickness=0.5, int resolution=20);
    };
}

