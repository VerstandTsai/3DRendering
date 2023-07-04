#include "proxima.h"

using namespace proxima;

int main() {
    int width = 1280;
    int height = 720;
    Window window(width, height);
    Renderer renderer(width, height);
    Scene scene;

    scene.camera.position = Vec3(0, 0, 8);
    scene.light_direction = rotate(Vec3(1, 0, 0), Vec3(0, 0, -45));
    scene["monkey"] = Object("suzanne.obj");
    scene["monkey"].position = Vec3(-5, 0, 0);
    scene["monkey"].color = Vec3(0.2, 0.2, 1);
    scene["cube"] = Cube();
    scene["cube"].position = Vec3(-2, 0, 0);
    scene["cube"].color = Vec3(1, 1, 0);
    scene["sphere"] = Sphere();
    scene["sphere"].position = Vec3(0, 0, 0);
    scene["sphere"].color = Vec3(1, 0.2, 0.2);
    scene["teapot"] = Object("teapot.obj");
    scene["teapot"].position = Vec3(5, 0, 0);
    scene["teapot"].color = Vec3(1, 0.5, 0.5);
    //scene["ball"] = Sphere(1, 36, Vec3(0, 0.5, 1));

    while (!window.closed()) {
        Vec3 right = cross(scene.camera.normal(), Vec3(0, 1, 0)).normalized();
        Vec3 front = cross(Vec3(0, 1, 0), right);

        if (window.keydown(W))
            scene.camera.position += front;
        if (window.keydown(S))
            scene.camera.position -= front;
        if (window.keydown(A))
            scene.camera.position -= right;
        if (window.keydown(D))
            scene.camera.position += right;
        if (window.keydown(SPACE))
            scene.camera.position += Vec3(0, 1, 0);
        if (window.keydown(LSHIFT))
            scene.camera.position -= Vec3(0, 1, 0);

        scene.camera.euler_angles -= Vec3(window.mouse_dy(), window.mouse_dx(), 0);

        scene["monkey"].euler_angles += Vec3(3, 4, 5);
        scene["cube"].euler_angles += Vec3(5, 4, 3);
        scene["sphere"].euler_angles += Vec3(0, 5, 0);
        scene["teapot"].euler_angles += Vec3(3, 5, 4);
        window.draw(renderer.render(scene));
    }

    return 0;
}

