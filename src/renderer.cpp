#include "renderer.h"
#include "objects.h"
#include "scene.h"
#include "mesh.h"
#include "texture.h"
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

    void Renderer::_init_fragment_buffer() {
        int half_width = this->_width >> 1;
        int half_height = this->_height >> 1;
        float z = -half_height / tan(deg2rad(this->_scene->camera.fov / 2));
        int index = 0;
        for (int y=half_height; y>-half_height; y--) {
            for (int x=-half_width; x<half_width; x++) {
                Fragment frag;
                frag.vision = -Vec3(x, y, z).normalized();
                this->_fragment_buffer[index] = frag;
                index++;
            }
        }
    }

    std::tuple<std::vector<Vertex*>, std::vector<Face>> make_vf(const Mesh &mesh) {
        std::vector<Vertex*> vertices;
        std::vector<Face> faces;
        if (mesh.has_normal()) {
            std::map<std::array<int, 3>, Vertex*> vertex_table;
            for (std::array<int, 9> face_index : mesh.face_indices()) {
                std::array<Vertex*, 3> face_vertices;
                for (int i=0; i<3; i++) {
                    int vi = face_index[i];
                    int ni = face_index[i+3];
                    int ti = (mesh.has_uv() ? face_index[i+6] : 0);
                    if (!vertex_table.contains({vi, ni, ti})) {
                        Vec3 v = mesh.vertices()[vi];
                        Vec3 n = mesh.vertex_normals()[ni];
                        Vec3 t = (mesh.has_uv() ? mesh.uv_coordinates()[ti] : Vec3());
                        vertex_table[{vi, ni, ti}] = new Vertex(v, n, t);
                    }
                    face_vertices[i] = vertex_table[{vi, ni, ti}];
                }
                faces.push_back(Face(face_vertices));
            }
            for (auto &entry : vertex_table) {
                vertices.push_back(entry.second);
            }
        } else {
            for (std::array<int, 9> face_index : mesh.face_indices()) {
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
                    Vec3 new_normal = lerp(a->normal, b->normal, t).normalized();
                    Vec3 new_uv = lerp(a->uv, b->uv, t);
                    Vec3 new_view_pos = lerp(a->view_pos, b->view_pos, t);
                    Vertex *new_vertex = new Vertex(new_pos, new_normal, new_uv, new_view_pos);
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

    void Renderer::_render_object(const Object &obj, bool is_skybox) {
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
            v->view_pos = modelview_matrix * (v->position * obj.scale);
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
            this->_rasterize(face, obj.texture, is_skybox, obj.is_light(), obj.shininess);
        }

        // Clean up
        for (Vertex *v : new_vertices) {
            delete v;
        }
    }

    void update_frag(Fragment &frag, Vec3 w, Face face, const Texture &texture, bool is_skybox, bool is_light, float shininess) {
        Vertex *va = face.vertices[0];
        Vertex *vb = face.vertices[1];
        Vertex *vc = face.vertices[2];

        // Perspective-correct barycentric coordinate
        Vec3 wp = Vec3(
            w.x / va->view_pos.z,
            w.y / vb->view_pos.z,
            w.z / vc->view_pos.z
        );
        wp /= dot(wp, Vec3(1, 1, 1));

        Vec3 uv =
              wp.x * va->uv
            + wp.y * vb->uv
            + wp.z * vc->uv;

        if (is_skybox) {
            frag.is_skybox = true;
            frag.color = texture.at_uv(uv);
            return;
        }

        float depth =
              w.x * va->position.z
            + w.y * vb->position.z
            + w.z * vc->position.z;
        if (depth > frag.depth) return;

        Vec3 normal = (
              wp.x * va->normal
            + wp.y * vb->normal
            + wp.z * vc->normal
        ).normalized();
        if (dot(normal, frag.vision) < 0) return;

        frag.depth = depth;
        frag.normal = normal;
        frag.is_skybox = false;
        frag.is_light = is_light;
        frag.shininess = shininess;
        frag.view_pos =
              wp.x * va->view_pos
            + wp.y * vb->view_pos
            + wp.z * vc->view_pos;
        frag.color = texture.at_uv(uv);
    }

    void Renderer::_rasterize(Face face, const Texture &texture, bool is_skybox, bool is_light, float shininess) {
        // Sort the vertices by y-value
        if (face.vertices[0]->position.y > face.vertices[1]->position.y)
            std::swap(face.vertices[0], face.vertices[1]);
        if (face.vertices[0]->position.y > face.vertices[2]->position.y)
            std::swap(face.vertices[0], face.vertices[2]);
        if (face.vertices[1]->position.y > face.vertices[2]->position.y)
            std::swap(face.vertices[1], face.vertices[2]);

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
                Vec3 w = lerp(wac, wb, tx); // The barycentric coordinate
                int index = x + y * this->_width;
                update_frag(this->_fragment_buffer[index], w, face, texture, is_skybox, is_light, shininess);
            }
        }
    }

    Vec3 Renderer::_shade(const Fragment &frag) {
        if (frag.is_skybox || frag.is_light) return frag.color;

        // Ambient reflection
        Vec3 ambient = this->_scene->ambient_light * Vec3(1, 1, 1);
        Vec3 diffuse = Vec3(0, 0, 0);
        Vec3 specular = Vec3(0, 0, 0);

        for (PointLight &light_source : this->_light_sources) {
            Vec3 frag_to_light = light_source.position - frag.view_pos;
            Vec3 light = frag_to_light.normalized();
            float distance = frag_to_light.magnitude();
            float intensity = light_source.intensity;
            Vec3 light_color = light_source.color * intensity / pow(distance, 2);

            // Diffuse reflection
            float ln = dot(light, frag.normal);
            if (ln < 0) continue;
            diffuse += ln * light_color;

            // Specular reflection
            Vec3 reflection = 2 * ln * frag.normal - light;
            float rv = dot(reflection, frag.vision);
            specular += pow(fmax(0, rv), frag.shininess) * (-1.0 / frag.shininess + 1) * light_color;
        }

        return (ambient + diffuse + specular) * frag.color;
    }

    int *Renderer::render(const Scene &scene) {
        this->_scene = &scene;
        this->_init_fragment_buffer();
        this->_calc_matrices();

        // Sort out the light sources
        this->_light_sources.clear();
        for (auto &obj_entry : scene.objects()) {
            if (obj_entry.second->is_light()) {
                PointLight light_source = *(PointLight*)(obj_entry.second);
                light_source.position = this->_view_matrix * light_source.position;
                this->_light_sources.push_back(light_source);
            }
        }

        // Create and render the skybox
        Object skybox(Mesh::Skybox(), scene.skybox);
        skybox.position = scene.camera.position;
        skybox.scale = Vec3(1, 1, 1) * scene.camera.far;
        this->_render_object(skybox, true);

        for (auto &obj_entry : scene.objects()) {
            this->_render_object(*obj_entry.second, false);
        }
        for (int i=0; i<this->_num_pixels; i++) {
            this->_frame_buffer[i] = color2rgba(this->_shade(this->_fragment_buffer[i]));
        }
        return this->_frame_buffer;
    }
}

