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

    Scene scene(Vec3(0.5, 0.8, 1));

    scene.camera.position = Vec3(0, 0, 8);

    scene["sun"] = new PointLight(10000, Vec3(1, 1, 1));
    scene["sun"]->position = Vec3(0, 100, 0);
    scene["light1"] = new PointLight(10, Vec3(1, 0, 0));
    scene["light1"]->position = Vec3(0, 5, 0);
    scene["light2"] = new PointLight(10, Vec3(0, 1, 0));
    scene["light2"]->position = Vec3(6, 0, 0);
    scene["light3"] = new PointLight(10, Vec3(0, 0, 1));
    scene["light3"]->position = Vec3(0, 7, 0);

    scene["monkey"] = new Object(Mesh("./static/suzanne.obj"));
    scene["monkey"]->position = Vec3(-5, 0, 0);
    scene["monkey"]->color = Vec3(0.2, 0.2, 1);

    scene["cuboid"] = new Object();
    scene["cuboid"]->position = Vec3(-2, 0, 0);
    scene["cuboid"]->color = Vec3(1, 1, 0);

    scene["sphere"] = new Object(Mesh::Sphere());
    scene["sphere"]->position = Vec3(0, 0, 0);
    scene["sphere"]->color = Vec3(1, 0.2, 0.2);

    scene["teapot"] = new Object(Mesh("./static/teapot.obj"));
    scene["teapot"]->position = Vec3(5, 0, 0);
    scene["teapot"]->color = Vec3(1, 0.5, 0.5);

    while (!window.closed()) {
        scene["light1"]->position = rotate(scene["light1"]->position, Vec3(1, 0, 0));
        scene["light2"]->position = rotate(scene["light2"]->position, Vec3(0, 1, 0));
        scene["light3"]->position = rotate(scene["light3"]->position, Vec3(0, 0, 1));

        scene["monkey"]->euler_angles += Vec3(3, 4, 5);
        scene["cuboid"]->euler_angles += Vec3(5, 4, 3);
        scene["sphere"]->euler_angles += Vec3(0, 5, 0);
        scene["teapot"]->euler_angles += Vec3(3, 5, 4);

        control(window, scene.camera);
        auto start = high_resolution_clock::now();
        window.draw(renderer.render(scene));
        auto end = high_resolution_clock::now();
        auto dur = duration_cast<milliseconds>(end - start);
        std::cout << "\rFPS: " << 1000 / dur.count() << std::flush;
    }

    return 0;
}

