#ifndef VEC_H
#define VEC_H

#include <array>
#include <iostream>

namespace proxima {
    float deg2rad(float degree);

    class Vec3 {
    public:
        float x, y, z;
        Vec3(float x=0, float y=0, float z=0) : x(x), y(y), z(z) {}
        Vec3 &operator+=(const Vec3 &v);
        Vec3 &operator-=(const Vec3 &v);
        Vec3 &operator*=(float a);
        Vec3 &operator/=(float a);
        float norm();
    };

    class Matrix3x3 {
    public:
        std::array<Vec3, 3> rows;
        Matrix3x3(std::array<Vec3, 3> rows);
    };

    Vec3 operator+(Vec3 a, const Vec3 &b);
    Vec3 operator-(Vec3 a, const Vec3 &b);
    Vec3 operator*(Vec3 v, float a);
    Vec3 operator*(float a, const Vec3 &v);
    Vec3 operator*(Matrix3x3 a, const Vec3 &v);
    Vec3 operator/(Vec3 v, float a);
    Vec3 operator/(float a, const Vec3 &v);
    Vec3 operator-(const Vec3 &v);
    std::ostream& operator<<(std::ostream& os, const Vec3 &v);

    float dot(Vec3 a, Vec3 b);
    Vec3 cross(Vec3 a, Vec3 b);

    Vec3 rotate(Vec3 v, Vec3 eulers);
}

#endif // VEC_H

