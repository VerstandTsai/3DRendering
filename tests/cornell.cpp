#include "proxima.h"
#include <chrono>

using namespace proxima;
using namespace std::chrono;

void control(Window &window, Camera &camera) {
    Vec3 cam_normal = rotate(Vec3(0, 0, -1), camera.euler_angles);
    Vec3 right = cross(cam_normal, Vec3(0, 1, 0)).normalized();
    Vec3 front = cross(Vec3(0, 1, 0), right);

    if (window.keydown(W))
        camera.position += front;
    if (window.keydown(S))
        camera.position -= front;
    if (window.keydown(A))
        camera.position -= right;
    if (window.keydown(D))
        camera.position += right;
    if (window.keydown(SPACE))
        camera.position += Vec3(0, 1, 0);
    if (window.keydown(LSHIFT))
        camera.position -= Vec3(0, 1, 0);

    camera.euler_angles -= Vec3(window.mouse_dy(), window.mouse_dx(), 0);
}

int main() {
    int width = 1280;
    int height = 720;
    Window window(width, height);
    Renderer renderer(width, height);

    Scene scene;
    scene.camera.position = Vec3(0, 0, 10);

    scene["light"] = new PointLight();
    scene["light"]->position = Vec3(0, 4.8, 0);

    scene["teapot"] = new Object(Mesh("./static/teapot.obj"));
    scene["teapot"]->scale = 1.5 * Vec3(1, 1, 1);
    scene["teapot"]->position = Vec3(0, -4.8, 0);

    scene["floor"] = new Object(Mesh::Cube());
    scene["floor"]->scale = Vec3(10, 0.1, 10);
    scene["floor"]->position = Vec3(0, -5, 0);

    scene["ceiling"] = new Object(Mesh::Cube());
    scene["ceiling"]->scale = Vec3(10, 0.1, 10);
    scene["ceiling"]->position = Vec3(0, 5, 0);

    scene["back"] = new Object(Mesh::Cube());
    scene["back"]->scale = Vec3(10, 10, 0.1);
    scene["back"]->position = Vec3(0, 0, -5);

    scene["left"] = new Object(Mesh::Cube(), Vec3(1, 0.2, 0.2));
    scene["left"]->scale = Vec3(0.1, 10, 10);
    scene["left"]->position = Vec3(-5, 0, 0);

    scene["right"] = new Object(Mesh::Cube(), Vec3(0.2, 1, 0.2));
    scene["right"]->scale = Vec3(0.1, 10, 10);
    scene["right"]->position = Vec3(5, 0, 0);

    float t = 0;
    while (!window.closed()) {
        scene["light"]->color = (Vec3(sin(t), sin(1.2*t), sin(0.8*t)) + Vec3(1, 1, 1)) / 2;
        t += 0.1;
        control(window, scene.camera);
        auto start = high_resolution_clock::now();
        window.draw(renderer.render(scene));
        auto end = high_resolution_clock::now();
        auto dur = duration_cast<milliseconds>(end - start);
        std::cout << "\rFPS: " << 1000 / dur.count() << std::flush;
    }

    return 0;
}

