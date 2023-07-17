#include "proxima.h"
using namespace proxima;

inline void control(Window &window, Camera &camera) {
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
    if (window.mouse_button_down(RIGHT))
        camera.fov = 30;
    else
        camera.fov = 90;

    camera.euler_angles -= Vec3(window.mouse_dy(), window.mouse_dx(), 0);
}

