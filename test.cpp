#include "proxima.h"

int main() {
    int width = 1280;
    int height = 720;
    proxima::Window window(width, height);
    proxima::Renderer renderer(width, height);
    proxima::Scene scene;

    scene.camera.position = proxima::Vec3(0, 0, 2);
    scene.light_direction = proxima::rotate(proxima::Vec3(0, -1, 0), proxima::Vec3(45, 45, 45));
    scene["cube"] = proxima::Cube();
    scene["cube"].color = proxima::Vec3(0, 0.5, 1);

    while (!window.closed()) {
        window.draw(renderer.render(scene));
        scene["cube"].euler_angles += proxima::Vec3(5, 5, 5);
    }

    return 0;
}

