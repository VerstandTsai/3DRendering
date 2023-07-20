#include "proxima.h"
#include "control.h"
#include "dashboard.h"

using namespace proxima;

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
    scene["teapot"]->scale = 1.2 * Vec3(1, 1, 1);
    scene["teapot"]->position = Vec3(0, -4.8, 0);
    scene["teapot"]->euler_angles = Vec3(0, 90, 0);

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

    while (!window.closed()) {
        control(window, scene.camera);
        window.draw(renderer.render(scene));
        show_fps();
    }

    return 0;
}

