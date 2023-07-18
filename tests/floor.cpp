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
    scene["floor"] = new Object();
    scene["floor"]->position = Vec3(0, -5, 0);
    scene["floor"]->scale = Vec3(100, 1, 100);
    while (!window.closed()) {
        control(window, scene.camera);
        window.draw(renderer.render(scene));
        show_fps();
    }
    return 0;
}

