#include "objects.h"
#include "vec3.h"

#include <fstream>
#include <sstream>
#include <string>

namespace proxima {
    Object::Object(Vec3 pos, Vec3 eulers, Vec3 color, int shininess, bool is_light) {
        this->position = pos;
        this->euler_angles = eulers;
        this->color = color;
        this->shininess = shininess;
        this->_is_light = is_light;
    }

    Object::Object(std::string filename) : Object() {
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
            if (cmd == "f") {
                int a, b, c;
                iss >> a >> b >> c;
                this->_face_indices.push_back({a-1, b-1, c-1});
            }
        }
    }

    Camera::Camera(float fov, Vec3 pos, Vec3 eulers) : Object(pos, eulers) {
        this->fov = fov;
    }

    Vec3 Camera::normal() {
        return rotate(Vec3(0, 0, -1), this->euler_angles);
    }

    PointLight::PointLight(float intensity, Vec3 color, Vec3 pos)
        : Cube(0.1, 0.1, 0.1, color, 1, pos, Vec3()) {
        this->_is_light = true;
        this->intensity = intensity;
    }

    Cube::Cube(float width, float height, float depth, Vec3 color, int shininess, Vec3 pos, Vec3 eulers)
        : Object(pos, eulers, color, shininess)
    {
        this->_width = width;
        this->_height = height;
        this->_depth = depth;
        for (int i=0; i<2; i++) {
            for (int j=0; j<2; j++) {
                for (int k=0; k<2; k++) {
                    float x = (i - 0.5) * width;
                    float y = (j - 0.5) * height;
                    float z = (k - 0.5) * depth;
                    this->_vertices.push_back(Vec3(x, y, z));
                }
            }
        }
        this->_face_indices.push_back({0, 1, 3});
        this->_face_indices.push_back({3, 2, 0});
        this->_face_indices.push_back({0, 2, 6});
        this->_face_indices.push_back({6, 4, 0});
        this->_face_indices.push_back({0, 4, 5});
        this->_face_indices.push_back({5, 1, 0});
        this->_face_indices.push_back({1, 5, 7});
        this->_face_indices.push_back({7, 3, 1});
        this->_face_indices.push_back({2, 3, 7});
        this->_face_indices.push_back({7, 6, 2});
        this->_face_indices.push_back({5, 4, 6});
        this->_face_indices.push_back({6, 7, 5});
    }

    Sphere::Sphere(float radius, int resolution, Vec3 color, int shininess, Vec3 pos, Vec3 eulers)
        : Object(pos, eulers, color, shininess)
    {
        this->_radius = radius;
        float theta = 180.0 / resolution;
        int num_v_ring = resolution * 2;
        int num_rings = resolution - 1;
        int num_v_total = num_v_ring * num_rings + 2;
        Vec3 v(0, radius, 0);
        for (int i=0; i<num_rings; i++) {
            v = rotate(v, Vec3(0, 0, theta));
            for (int j=0; j<num_v_ring; j++) {
                v = rotate(v, Vec3(0, theta, 0));
                this->_vertices.push_back(v);
                int base_index = i * num_v_ring;
                int a, b, c, d;
                b = j + base_index;
                c = (j+1) % num_v_ring + base_index;
                a = b - num_v_ring;
                d = c - num_v_ring;
                if (i == 0) {
                    this->_face_indices.push_back({num_v_total-2, b, c});
                    continue;
                }
                this->_face_indices.push_back({a, b, c});
                this->_face_indices.push_back({c, d, a});
                if (i == num_rings - 1) {
                    this->_face_indices.push_back({num_v_total-1, c, b});
                }
            }
        }
        this->_vertices.push_back(Vec3(0, radius, 0));
        this->_vertices.push_back(Vec3(0, -radius, 0));
    }
}

