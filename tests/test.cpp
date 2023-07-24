#include "proxima.h"
#include "control.h"
#include "dashboard.h"

using namespace proxima;

int main() {
    int width = 1280;
    int height = 720;
    Window window(width, height);
    Renderer renderer(width, height);

    Scene scene(Texture("./static/skybox.png"));

    scene.camera.position = Vec3(0, 0, 8);

    scene["sun"] = new PointLight(10000, Vec3(1, 1, 1));
    scene["sun"]->position = Vec3(0, 100, 0);
    scene["light1"] = new PointLight(10, Vec3(1, 0, 0));
    scene["light1"]->position = Vec3(0, 5, 0);
    scene["light2"] = new PointLight(10, Vec3(0, 1, 0));
    scene["light2"]->position = Vec3(6, 0, 0);
    scene["light3"] = new PointLight(10, Vec3(0, 0, 1));
    scene["light3"]->position = Vec3(0, 7, 0);

    scene["donut"] = new Object(Mesh::Torus(), Texture::Checker(16, 8));
    scene["donut"]->position = Vec3(0, 5, 0);

    scene["monkey"] = new Object(Mesh("./static/suzanne.obj"), Texture("./static/suzanne_texture.png"));
    scene["monkey"]->position = Vec3(-5, 0, 0);

    scene["cuboid"] = new Object(Mesh::Cube(), Texture("./static/cube_texture.png"));
    scene["cuboid"]->position = Vec3(-2, 0, 0);

    scene["sphere"] = new Object(Mesh::Sphere(), Texture("./static/map.png"));
    scene["sphere"]->position = Vec3(0, 0, 0);

    scene["teapot"] = new Object(Mesh("./static/teapot.obj"), Texture::Color(Vec3(0.8, 0.8, 0.8)));
    scene["teapot"]->position = Vec3(5, 0, 0);
    scene["teapot"]->euler_angles = Vec3(0, 90, 0);

    while (!window.closed()) {
        scene["light1"]->position = rotate(scene["light1"]->position, Vec3(1, 0, 0));
        scene["light2"]->position = rotate(scene["light2"]->position, Vec3(0, 1, 0));
        scene["light3"]->position = rotate(scene["light3"]->position, Vec3(0, 0, 1));

        scene["donut"]->euler_angles += Vec3(4, 0, 2);
        scene["sphere"]->euler_angles += Vec3(0, 5, 0);

        control(window, scene.camera);
        window.draw(renderer.render(scene));
        show_fps();
    }

    return 0;
}

