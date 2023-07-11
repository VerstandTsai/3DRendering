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
        Vec3 &operator*=(const Vec3 &v);
        Vec3 &operator*=(float a);
        Vec3 &operator/=(float a);
        float magnitude();
        Vec3 normalized();
    };

    Vec3 operator+(Vec3 a, const Vec3 &b);
    Vec3 operator-(Vec3 a, const Vec3 &b);
    Vec3 operator*(Vec3 v, float a);
    Vec3 operator*(float a, const Vec3 &v);
    Vec3 operator*(Vec3 a, const Vec3 &b);
    Vec3 operator/(Vec3 v, float a);
    Vec3 operator/(float a, const Vec3 &v);
    Vec3 operator-(const Vec3 &v);
    std::ostream& operator<<(std::ostream& os, const Vec3 &v);

    float dot(Vec3 a, Vec3 b);
    Vec3 cross(Vec3 a, Vec3 b);
    Vec3 rotate(Vec3 v, Vec3 eulers);
    Vec3 lerp(Vec3 a, Vec3 b, float t);
    float lerp(float a, float b, float t);

    class Vec4 {
    public:
        float x, y, z, w;
        Vec4(float x=0, float y=0, float z=0, float w=1) : x(x), y(y), z(z), w(w) {}
        Vec4(Vec3 v) : Vec4(v.x, v.y, v.z) {}
        operator Vec3() const { return Vec3(this->x, this->y, this->z) / this->w; }
    };

    class Mat4 {
    private:
        std::array<std::array<float, 4>, 4> _rows;

    public:
        Mat4();
        Mat4(std::array<std::array<float, 4>, 4> rows);
        static Mat4 Identity();
        static Mat4 Translation(Vec3 displacement);
        static Mat4 Scale(Vec3 factors);
        static Mat4 RotX(float theta);
        static Mat4 RotY(float theta);
        static Mat4 RotZ(float theta);
        std::array<float, 4> &operator[](int i);
    };

    Vec4 operator*(Mat4 a, const Vec4 &v);
    Mat4 operator*(Mat4 a, Mat4 b);
}

#endif // VEC_H

