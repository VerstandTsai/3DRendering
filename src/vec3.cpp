#include "vec3.h"
#include <array>
#include <cmath>

namespace proxima {
    Mat4::Mat4() : Mat4({{
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }}) {}

    Mat4::Mat4(std::array<std::array<float, 4>, 4> rows) {
        this->_rows = rows;
    }

    Mat4 Mat4::Identity() {
        return Mat4({{
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1},
        }});
    }

    Mat4 Mat4::Translation(Vec3 displacement) {
        float x = displacement.x;
        float y = displacement.y;
        float z = displacement.z;
        return Mat4({{
            {1, 0, 0, x},
            {0, 1, 0, y},
            {0, 0, 1, z},
            {0, 0, 0, 1},
        }});
    }

    Mat4 Mat4::Scale(Vec3 factors) {
        float x = factors.x;
        float y = factors.y;
        float z = factors.z;
        return Mat4({{
            {x, 0, 0, 0},
            {0, y, 0, 0},
            {0, 0, z, 0},
            {0, 0, 0, 1},
        }});
    }

    Mat4 Mat4::RotX(float theta) {
        float sin0 = sin(theta);
        float cos0 = cos(theta);
        return Mat4({{
            {1,    0,     0, 0},
            {0, cos0, -sin0, 0},
            {0, sin0,  cos0, 0},
            {0,    0,     0, 1}
        }});
    }

    Mat4 Mat4::RotY(float theta) {
        float sin0 = sin(theta);
        float cos0 = cos(theta);
        return Mat4({{
            { cos0, 0,  sin0, 0},
            {    0, 1,     0, 0},
            {-sin0, 0,  cos0, 0},
            {    0, 0,     0, 1}
        }});
    }

    Mat4 Mat4::RotZ(float theta) {
        float sin0 = sin(theta);
        float cos0 = cos(theta);
        return Mat4({{
            {cos0, -sin0, 0, 0},
            {sin0,  cos0, 0, 0},
            {   0,     0, 1, 0},
            {   0,     0, 0, 1}
        }});
    }

    std::array<float, 4> &Mat4::operator[](int i) {
        return this->_rows[i];
    }

    Vec4 operator*(Mat4 a, const Vec4 &v) {
        float elements[4];
        for (int i=0; i<4; i++)
            elements[i] = a[i][0] * v.x + a[i][1] * v.y + a[i][2] * v.z + a[i][3] * v.w;
        return Vec4(elements[0], elements[1], elements[2], elements[3]);
    }

    Mat4 operator*(Mat4 a, Mat4 b) {
        Mat4 result;
        for (int i=0; i<4; i++) {
            for (int k=0; k<4; k++) {
                for (int j=0; j<4; j++) {
                    result[i][j] += a[i][k] * b[k][j];
                }
            }
        }
        return result;
    }

    Vec4 lerp(Vec4 a, Vec4 b, float t) {
        return Vec4(
            std::lerp(a.x, b.x, t),
            std::lerp(a.y, b.y, t),
            std::lerp(a.z, b.z, t),
            std::lerp(a.w, b.w, t)
        );
    }

    Vec3 rotate(Vec3 v, Vec3 eulers) {
        float x = deg2rad(eulers.x);
        float y = deg2rad(eulers.y);
        float z = deg2rad(eulers.z);
        return Mat4::RotZ(z) * Mat4::RotY(y) * Mat4::RotX(x) * v;
    }
}

