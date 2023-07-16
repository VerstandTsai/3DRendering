#include "renderer.h"
#include "objects.h"
#include "scene.h"
#include "mesh.h"
#include "vec3.h"
#include <cmath>
#include <algorithm>
#include <vector>
#include <array>
#include <set>
#include <tuple>

namespace proxima {
    int color2rgba(Vec3 color) {
        int r = fmin(1, color.x) * 255;
        int g = fmin(1, color.y) * 255;
        int b = fmin(1, color.z) * 255;
        return (r << 24) + (g << 16) + (b << 8) + 0xff;
    }

    Renderer::Renderer(int width, int height) {
        this->_width = width;
        this->_height = height;
        this->_num_pixels = width * height;
        this->_aspect = (float)width / height;
        this->_frame_buffer = new int[this->_num_pixels];
        this->_fragment_buffer = new Fragment[this->_num_pixels];
    }

    Renderer::~Renderer() {
        delete [] this->_frame_buffer;
        delete [] this->_fragment_buffer;
    }

    void Renderer::_calc_matrices() {
        Camera cam = this->_scene->camera;
        float n = cam.near;
        float f = cam.far;
        float s = 1 / tan(deg2rad(cam.fov / 2));
        float x = deg2rad(cam.euler_angles.x);
        float y = deg2rad(cam.euler_angles.y);
        float a = 1 / this->_aspect;
        this->_view_rotation =
              Mat4::RotX(-x)
            * Mat4::RotY(-y);
        this->_view_matrix = this->_view_rotation * Mat4::Translation(-cam.position);
        this->_projection_matrix = Mat4({{{
            {a*s, 0,        0,          0},
            {  0, s,        0,          0},
            {  0, 0, -f/(f-n), -f*n/(f-n)},
            {  0, 0,       -1,          0}
        }}});
    }

    void Renderer::_calc_visions() {
        int half_width = this->_width >> 1;
        int half_height = this->_height >> 1;
        float z = -half_height / tan(deg2rad(this->_scene->camera.fov / 2));
        int index = 0;
        for (int y=half_height; y>-half_height; y--) {
            for (int x=-half_width; x<half_width; x++) {
                this->_fragment_buffer[index].vision = -Vec3(x, y, z).normalized();
                index++;
            }
        }
    }

    std::tuple<std::vector<Vertex*>, std::vector<Face>> make_vf(const Mesh &mesh) {
        std::vector<Vertex*> vertices;
        std::vector<Face> faces;
        if (mesh.has_normal()) {
            std::map<std::array<int, 2>, Vertex*> vertex_table;
            for (std::array<int, 6> face_index : mesh.face_indices()) {
                std::array<Vertex*, 3> face_vertices;
                for (int i=0; i<3; i++) {
                    int vi = face_index[i];
                    int ni = face_index[i+3];
                    if (!vertex_table.contains({vi, ni})) {  
                        Vec3 v = mesh.vertices()[vi];
                        Vec3 n = mesh.vertex_normals()[ni];
                        vertex_table[{vi, ni}] = new Vertex(v, n);
                    }
                    face_vertices[i] = vertex_table[{vi, ni}];
                }
                faces.push_back(Face(face_vertices));
            }
            for (auto &entry : vertex_table) {
                vertices.push_back(entry.second);
            }
        } else {
            for (std::array<int, 6> face_index : mesh.face_indices()) {
                Vec3 a = mesh.vertices()[face_index[0]];
                Vec3 b = mesh.vertices()[face_index[1]];
                Vec3 c = mesh.vertices()[face_index[2]];
                Vec3 normal = cross(b-a, c-a).normalized();
                Vertex *va = new Vertex(a, normal);
                Vertex *vb = new Vertex(b, normal);
                Vertex *vc = new Vertex(c, normal);
                vertices.push_back(va);
                vertices.push_back(vb);
                vertices.push_back(vc);
                faces.push_back(Face({va, vb, vc}));
            }
        }
        return {vertices, faces};
    }

    std::tuple<std::set<Vertex*>, std::vector<Face>> clip_faces(const std::vector<Face> &faces) {
        std::vector<Face> new_faces;
        std::set<Vertex*> new_vertices;
        std::set<Vertex*> old_vertices;
        for (Face face : faces) {
            std::vector<Vertex*> face_vertices;
            for (int i=0; i<3; i++) {
                Vertex *a = face.vertices[i];
                Vertex *b = face.vertices[(i+1)%3];
                if (a->position.z > 0) {
                    face_vertices.push_back(a);
                    new_vertices.insert(a);
                } else {
                    old_vertices.insert(a);
                }
                if (a->position.z * b->position.z < 0) {
                    float t = a->position.z / (a->position.z - b->position.z);
                    Vec4 new_pos = lerp(a->position, b->position, t);
                    Vec3 new_normal = lerp(a->normal, b->normal, t);
                    Vec3 new_view_pos = lerp(a->view_pos, b->view_pos, t);
                    Vertex *new_vertex = new Vertex(new_pos, new_normal, new_view_pos);
                    face_vertices.push_back(new_vertex);
                    new_vertices.insert(new_vertex);
                }
            }
            if (face_vertices.size() == 0) continue;
            new_faces.push_back(Face({face_vertices[0], face_vertices[1], face_vertices[2]}));
            if (face_vertices.size() == 4)
                new_faces.push_back(Face({face_vertices[3], face_vertices[0], face_vertices[2]}));
        }

        for (Vertex *v : old_vertices) {
            delete v;
        }

        return {new_vertices, new_faces};
    }

    void Renderer::_render_object(const Object &obj) {
        auto [vertices, faces] = make_vf(obj.mesh());

        // Project the vertices to clip space
        float x = deg2rad(obj.euler_angles.x);
        float y = deg2rad(obj.euler_angles.y);
        float z = deg2rad(obj.euler_angles.z);
        Mat4 model_rotation =
              Mat4::RotZ(z)
            * Mat4::RotY(y)
            * Mat4::RotX(x);
        Mat4 model_matrix = Mat4::Translation(obj.position) * model_rotation;
        Mat4 modelview_matrix = this->_view_matrix * model_matrix;
        Mat4 modelview_rotation = this->_view_rotation * model_rotation;
        for (Vertex *v : vertices) {
            v->normal = modelview_rotation * v->normal;
            v->view_pos = modelview_matrix * v->position;
            v->position = this->_projection_matrix * v->view_pos;
        }

        auto [new_vertices, new_faces] = clip_faces(faces);

        // Transform to screen space
        for (Vertex *v : new_vertices) {
            Vec3 ndc_space = v->position;
            int half_width = this->_width >> 1;
            int half_height = this->_height >> 1;
            int screen_x = (ndc_space.x + 1) * half_width;
            int screen_y = (-ndc_space.y + 1) * half_height;
            v->position = Vec3(screen_x, screen_y, ndc_space.z);
        }

        // Create the fragments
        for (Face face : new_faces) {
            this->_rasterize(face, obj.color, obj.is_light(), obj.shininess);
        }

        // Clean up
        for (Vertex *v : new_vertices) {
            delete v;
        }
    }

    void Renderer::_rasterize(Face face, Vec3 color, bool is_light, float shininess) {
        std::sort(face.vertices.begin(), face.vertices.end(),
            [](Vertex *a, Vertex *b) {
                return a->position.y < b->position.y;
            }
        );
        Vec3 a = face.vertices[0]->position;
        Vec3 b = face.vertices[1]->position;
        Vec3 c = face.vertices[2]->position;
        for (int y=fmax(0, a.y); y<fmin(this->_height, c.y); y++) {
            float tac = (float)(y - a.y) / (c.y - a.y);
            Vec3 wac = lerp(Vec3(1, 0, 0), Vec3(0, 0, 1), tac);
            Vec3 wb;
            int xac = std::lerp(a.x, c.x, tac);
            int xb;
            if (y < b.y) {
                float tab = (float)(y - a.y) / (b.y - a.y);
                wb = lerp(Vec3(1, 0, 0), Vec3(0, 1, 0), tab);
                xb = std::lerp(a.x, b.x, tab);
            } else {
                float tbc = (float)(y - b.y) / (c.y - b.y);
                wb = lerp(Vec3(0, 1, 0), Vec3(0, 0, 1), tbc);
                xb = std::lerp(b.x, c.x, tbc);
            }
            int xmin = fmin(xac, xb);
            int xmax = fmax(xac, xb);
            for (int x=fmax(0, xmin); x<fmin(this->_width, xmax); x++) {
                float tx = (float)(x - xac) / (xb - xac);
                Vec3 w = lerp(wac, wb, tx);
                int index = x + y * this->_width;
                Fragment &frag = this->_fragment_buffer[index];

                float depth =
                      w.x * a.z
                    + w.y * b.z
                    + w.z * c.z;
                if (depth > frag.depth) continue;

                Vec3 normal =
                      w.x * face.vertices[0]->normal
                    + w.y * face.vertices[1]->normal
                    + w.z * face.vertices[2]->normal;
                if (dot(normal, frag.vision) < 0) continue;

                frag.depth = depth;
                frag.normal = normal;
                frag.color = color;
                frag.is_light = is_light;
                frag.shininess = shininess;
                frag.is_nothing = false;
                frag.view_pos =
                      w.x * face.vertices[0]->view_pos
                    + w.y * face.vertices[1]->view_pos
                    + w.z * face.vertices[2]->view_pos;
            }
        }
    }

    Vec3 Renderer::_shade(const Fragment &frag) {
        if (frag.is_nothing) return this->_scene->bg_color;
        if (frag.is_light) return frag.color;

        // Ambient reflection
        Vec3 ambient = this->_scene->ambient_light * Vec3(1, 1, 1);
        Vec3 diffuse = Vec3(0, 0, 0);
        Vec3 specular = Vec3(0, 0, 0);

        for (PointLight &light_source : this->_light_sources) {
            Vec3 face_to_light = light_source.position - frag.view_pos;
            Vec3 light = face_to_light.normalized();
            float distance = face_to_light.magnitude();
            float intensity = light_source.intensity;
            Vec3 light_color = light_source.color * intensity / pow(distance, 2);

            // Diffuse reflection
            float ln = dot(light, frag.normal);
            diffuse += fmax(0, ln) * light_color;

            // Specular reflection
            Vec3 reflection = 2 * ln * frag.normal - light;
            float rv = dot(reflection, frag.vision);
            specular += pow(fmax(0, rv), frag.shininess) * (-1.0 / frag.shininess + 1) * light_color;
        }

        return (ambient + diffuse + specular) * frag.color;
    }

    int *Renderer::render(const Scene &scene) {
        for (int i=0; i<this->_num_pixels; i++) {
            this->_fragment_buffer[i] = Fragment();
            this->_frame_buffer[i] = color2rgba(scene.bg_color);
        }
        this->_scene = &scene;
        this->_calc_visions();
        this->_calc_matrices();
        this->_light_sources.clear();
        for (auto &obj_entry : scene.objects()) {
            if (obj_entry.second->is_light()) {
                PointLight light_source = *(PointLight*)(obj_entry.second);
                light_source.position = this->_view_matrix * light_source.position;
                this->_light_sources.push_back(light_source);
            }
        }
        for (auto &obj_entry : scene.objects()) {
            this->_render_object(*obj_entry.second);
        }
        for (int i=0; i<this->_num_pixels; i++) {
            this->_frame_buffer[i] = color2rgba(this->_shade(this->_fragment_buffer[i]));
        }
        return this->_frame_buffer;
    }
}

