#include "mesh.h"
#include "vec3.h"

#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>

namespace proxima {
    Mesh::Mesh(std::string filename) : Mesh() {
        std::ifstream infile(filename);
        std::string line;
        while (std::getline(infile, line)) {
            std::istringstream iss(line);
            std::string cmd;
            iss >> cmd;
            if (cmd == "v") {
                float x, y, z;
                iss >> x >> y >> z;
                this->_vertices.push_back(Vec3(x, y, z));
            }
            if (cmd == "vn") {
                this->_has_normal = true;
                float x, y, z;
                iss >> x >> y >> z;
                this->_vertex_normals.push_back(Vec3(x, y, z).normalized());
            }
            if (cmd == "f") {
                if (this->_has_normal) {
                    std::array<int, 6> indices;
                    for (int i=0; i<3; i++) {
                        std::string index_pair;
                        iss >> index_pair;
                        int vertex, normal;
                        sscanf(index_pair.c_str(), "%d//%d", &vertex, &normal);
                        indices[i] = vertex - 1;
                        indices[i+3] = normal - 1;
                    }
                    this->_face_indices.push_back(indices);
                } else {
                    int a, b, c;
                    iss >> a >> b >> c;
                    this->_face_indices.push_back({a-1, b-1, c-1});
                }
            }
        }
    }

    Mesh Mesh::Cube() {
        Mesh mesh;
        for (int i=0; i<2; i++) {
            for (int j=0; j<2; j++) {
                for (int k=0; k<2; k++) {
                    mesh._vertices.push_back(Vec3(i, j, k) - Vec3(0.5, 0.5, 0.5));
                }
            }
        }
        mesh._face_indices.push_back({0, 1, 3});
        mesh._face_indices.push_back({3, 2, 0});
        mesh._face_indices.push_back({0, 2, 6});
        mesh._face_indices.push_back({6, 4, 0});
        mesh._face_indices.push_back({0, 4, 5});
        mesh._face_indices.push_back({5, 1, 0});
        mesh._face_indices.push_back({1, 5, 7});
        mesh._face_indices.push_back({7, 3, 1});
        mesh._face_indices.push_back({2, 3, 7});
        mesh._face_indices.push_back({7, 6, 2});
        mesh._face_indices.push_back({5, 4, 6});
        mesh._face_indices.push_back({6, 7, 5});
        return mesh;
    }

    Mesh Mesh::Sphere(int resolution) {
        Mesh mesh;
        mesh._has_normal = true;
        float theta = 180.0 / resolution;
        int num_v_ring = resolution * 2;
        int num_rings = resolution - 1;
        int num_v_total = num_v_ring * num_rings + 2;
        Vec3 v(0, 1, 0);
        for (int i=0; i<num_rings; i++) {
            v = rotate(v, Vec3(0, 0, theta));
            for (int j=0; j<num_v_ring; j++) {
                v = rotate(v, Vec3(0, theta, 0));
                mesh._vertices.push_back(v);
                mesh._vertex_normals.push_back(v);
                int base_index = i * num_v_ring;
                int a, b, c, d;
                b = j + base_index;
                c = (j+1) % num_v_ring + base_index;
                a = b - num_v_ring;
                d = c - num_v_ring;
                if (i == 0) {
                    int top = num_v_total - 2;
                    mesh._face_indices.push_back({top, b, c, top, b, c});
                    continue;
                }
                mesh._face_indices.push_back({a, b, c, a, b, c});
                mesh._face_indices.push_back({c, d, a, c, d, a});
                if (i == num_rings - 1) {
                    int bottom = num_v_total - 1;
                    mesh._face_indices.push_back({bottom, c, b, bottom, c, b});
                }
            }
        }
        mesh._vertices.push_back(Vec3(0, 1, 0));
        mesh._vertices.push_back(Vec3(0, -1, 0));
        mesh._vertex_normals.push_back((Vec3(0, 1, 0)));
        mesh._vertex_normals.push_back((Vec3(0, -1, 0)));
        return mesh;
    }
}
