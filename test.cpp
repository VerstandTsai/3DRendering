#include "proxima.h"

int main() {
    int width = 1280;
    int height = 720;
    proxima::Window window(width, height);
    proxima::Renderer renderer(width, height);

    renderer.camera.position = proxima::Vec3(0, 0, 2);
    proxima::Cube cube(proxima::Vec3(0, 0, 0));
    cube.euler_angles = proxima::Vec3(0, 0, 0);

    while (!window.closed()) {
        window.draw(renderer.render(cube));
        cube.euler_angles += proxima::Vec3(10, 10, 10);
    }

    return 0;
}

