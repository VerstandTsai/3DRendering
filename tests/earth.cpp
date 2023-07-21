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

    scene["sun"] = new PointLight(200);
    scene["earth"] = new Object(Mesh::Sphere(), Texture("./static/map.ppm"));
    scene["earth"]->position = Vec3(10, 0, 0);
    scene["earth"]->euler_angles = Vec3(0, 0, 23.5);

    while (!window.closed()) {
        scene["earth"]->position = rotate(scene["earth"]->position, Vec3(0, 1, 0));
        scene["earth"]->euler_angles += Vec3(0, 5, 0);
        control(window, scene.camera);
        window.draw(renderer.render(scene));
        show_fps();
    }

    return 0;
}

