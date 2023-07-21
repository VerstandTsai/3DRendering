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
                    std::array<int, 9> indices;
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
        mesh._has_uv = true;

        float theta = 180.0 / resolution;
        int num_v_ring = resolution * 2;
        int num_rings = resolution - 1;
        int num_v_no_poles = num_v_ring * num_rings;

        int north = num_v_no_poles;
        int south = num_v_no_poles + 1;

        int uv_index_right = num_v_no_poles;
        int uv_index_north = uv_index_right + num_rings;
        int uv_index_south = uv_index_north + num_v_ring;

        float delta_u = 1.0 / num_v_ring;
        float delta_v = 1.0 / resolution;

        Vec3 v(0, 1, 0);
        for (int i=0; i<num_rings; i++) {
            v = rotate(v, Vec3(0, 0, theta));
            for (int j=0; j<num_v_ring; j++) {
                mesh._vertices.push_back(v);
                mesh._vertex_normals.push_back(v);
                mesh._uv_coordinates.push_back(Vec3(j * delta_u, (i + 1) * delta_v, 0));

                int base_index = i * num_v_ring;
                int a, b, c, d;
                b = j + base_index;
                c = (j+1) % num_v_ring + base_index;
                a = b - num_v_ring;
                d = c - num_v_ring;

                int c_uv = c;
                int d_uv = d;
                if (j == num_v_ring - 1) {
                    c_uv = uv_index_right + i;
                    d_uv = c_uv - 1;
                }

                if (i == 0) {
                    mesh._face_indices.push_back({
                        north, b, c,
                        north, b, c,
                        uv_index_north + j, b, c_uv
                    });
                } else {
                    mesh._face_indices.push_back({a, b, c, a, b, c, a, b, c_uv});
                    mesh._face_indices.push_back({c, d, a, c, d, a, c_uv, d_uv, a});
                    if (i == num_rings - 1) {
                        mesh._face_indices.push_back({
                            south, c, b,
                            south, c, b,
                            uv_index_south + j, c_uv, b
                        });
                    }
                }
                v = rotate(v, Vec3(0, theta, 0));
            }
        }
        mesh._vertices.push_back(Vec3(0, 1, 0));
        mesh._vertices.push_back(Vec3(0, -1, 0));
        mesh._vertex_normals.push_back((Vec3(0, 1, 0)));
        mesh._vertex_normals.push_back((Vec3(0, -1, 0)));
        for (int i=0; i<num_rings; i++) {
            mesh._uv_coordinates.push_back(Vec3(1, (i + 1) * delta_v, 0));
        }
        for (int i=0; i<num_v_ring; i++) {
            mesh._uv_coordinates.push_back(Vec3((i + 0.5) * delta_u, 0, 0));
        }
        for (int i=0; i<num_v_ring; i++) {
            mesh._uv_coordinates.push_back(Vec3((i + 0.5) * delta_u, 1, 0));
        }
        return mesh;
    }

    Mesh Mesh::Torus(float thickness, int resolution) {
        Mesh mesh;
        mesh._has_normal = true;
        int num_v_ring = resolution;
        int num_rings = resolution / thickness;
        float theta = 360.0 / num_v_ring;
        float alpha = 360.0 / num_rings;
        std::vector<Vec3> circle_v;
        std::vector<Vec3> circle_n;
        Vec3 v = Vec3(1, 0, 0);
        for (int i=0; i<num_v_ring; i++) {
            circle_n.push_back(v);
            circle_v.push_back(Vec3(1, 0, 0) + v * thickness);
            v = rotate(v, Vec3(0, 0, theta));
        }
        for (int i=0; i<num_rings; i++) {
            Vec3 eulers = Vec3(0, alpha * i, 0);
            for (int j=0; j<num_v_ring; j++) {
                mesh._vertex_normals.push_back(rotate(circle_n[j], eulers));
                mesh._vertices.push_back(rotate(circle_v[j], eulers));
                int i_next = (i + 1) % num_rings;
                int j_next = (j + 1) % num_v_ring;
                int a = j + i * num_v_ring;
                int b = j + i_next * num_v_ring;
                int c = j_next + i_next * num_v_ring;
                int d = j_next + i * num_v_ring;
                mesh._face_indices.push_back({a, b, c, a, b, c});
                mesh._face_indices.push_back({c, d, a, c, d, a});
            }
        }
        return mesh;
    }
}

