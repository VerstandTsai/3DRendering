#include "proxima.h"

int main() {
    int width = 1280;
    int height = 720;
    proxima::Window window(width, height);
    proxima::Renderer renderer(width, height);
    proxima::Scene scene;

    scene.camera.position = proxima::Vec3(0, 0, 2);
    scene["cube"] = proxima::Cube();

    while (!window.closed()) {
        window.draw(renderer.render(scene));
        scene["cube"].euler_angles += proxima::Vec3(10, 10, 10);
    }

    return 0;
}

