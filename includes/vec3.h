#pragma once

#include <array>
#include <cmath>
#include <iostream>

namespace proxima {
    inline float deg2rad(float degree) {
        return degree / 180 * M_PI;
    }

    class Vec3 {
    public:
        float x, y, z;
        Vec3(float x=0, float y=0, float z=0) : x(x), y(y), z(z) {}
        inline Vec3 &operator+=(const Vec3 &v);
        inline Vec3 &operator-=(const Vec3 &v);
        inline Vec3 &operator*=(const Vec3 &v);
        inline Vec3 &operator*=(float a);
        inline Vec3 &operator/=(float a);
        inline float magnitude();
        inline Vec3 normalized();
    };

    inline Vec3 operator+(Vec3 a, const Vec3 &b);
    inline Vec3 operator-(Vec3 a, const Vec3 &b);
    inline Vec3 operator*(Vec3 v, float a);
    inline Vec3 operator*(float a, const Vec3 &v);
    inline Vec3 operator*(Vec3 a, const Vec3 &b);
    inline Vec3 operator/(Vec3 v, float a);
    inline Vec3 operator/(float a, const Vec3 &v);
    inline Vec3 operator-(const Vec3 &v);
    inline std::ostream& operator<<(std::ostream& os, const Vec3 &v);

    inline float dot(Vec3 a, Vec3 b);
    inline Vec3 cross(Vec3 a, Vec3 b);
    inline Vec3 lerp(Vec3 a, Vec3 b, float t);
    Vec3 rotate(Vec3 v, Vec3 eulers);

    class Vec4 {
    public:
        float x, y, z, w;
        Vec4(float x=0, float y=0, float z=0, float w=1) : x(x), y(y), z(z), w(w) {}
        Vec4(Vec3 v) : Vec4(v.x, v.y, v.z) {}
        inline operator Vec3() const { return Vec3(this->x, this->y, this->z) / this->w; }
    };

    Vec4 lerp(Vec4 a, Vec4 b, float t);

    class Mat4 {
    private:
        std::array<std::array<float, 4>, 4> _rows;

    public:
        Mat4();
        Mat4(std::array<std::array<float, 4>, 4> rows);
        static Mat4 Translation(Vec3 displacement);
        static Mat4 Scale(Vec3 factors);
        static Mat4 RotX(float theta);
        static Mat4 RotY(float theta);
        static Mat4 RotZ(float theta);
        std::array<float, 4> &operator[](int i);
    };

    Vec4 operator*(Mat4 a, const Vec4 &v);
    Mat4 operator*(Mat4 a, Mat4 b);

    float Vec3::magnitude() {
        return sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
    }

    Vec3 Vec3::normalized() {
        return *this / this->magnitude();
    }

    Vec3 &Vec3::operator+=(const Vec3 &v) {
        this->x += v.x;
        this->y += v.y;
        this->z += v.z;
        return *this;
    }

    Vec3 &Vec3::operator-=(const Vec3 &v) {
        this->x -= v.x;
        this->y -= v.y;
        this->z -= v.z;
        return *this;
    }

    Vec3 &Vec3::operator*=(float a) {
        this->x *= a;
        this->y *= a;
        this->z *= a;
        return *this;
    }

    Vec3 &Vec3::operator*=(const Vec3 &v) {
        this->x *= v.x;
        this->y *= v.y;
        this->z *= v.z;
        return *this;
    }

    Vec3 &Vec3::operator/=(float a) {
        this->x /= a;
        this->y /= a;
        this->z /= a;
        return *this;
    }

    Vec3 operator+(Vec3 a, const Vec3 &b) {
        a += b;
        return a;
    }

    Vec3 operator-(Vec3 a, const Vec3 &b) {
        a -= b;
        return a;
    }

    Vec3 operator*(Vec3 v, float a) {
        v *= a;
        return v;
    }

    Vec3 operator*(float a, const Vec3 &v) {
        return v * a;
    }

    Vec3 operator*(Vec3 a, const Vec3 &b) {
        a *= b;
        return a;
    }

    Vec3 operator/(Vec3 v, float a) {
        v /= a;
        return v;
    }

    Vec3 operator/(float a, const Vec3 &v) {
        return v / a;
    }

    Vec3 operator-(const Vec3 &v) {
        return Vec3(0, 0, 0) - v;
    }

    std::ostream& operator<<(std::ostream& os, const Vec3 &v) {
        os << '[' << v.x << ", " << v.y << ", " << v.z << ']';
        return os;
    }

    float dot(Vec3 a, Vec3 b) {
        return a.x*b.x + a.y*b.y + a.z*b.z;
    }

    Vec3 cross(Vec3 a, Vec3 b) {
        Vec3 v(0, 0, 0);
        v.x = a.y*b.z - a.z*b.y;
        v.y = a.z*b.x - a.x*b.z;
        v.z = a.x*b.y - a.y*b.x;
        return v;
    }

    Vec3 lerp(Vec3 a, Vec3 b, float t) {
        return (1 - t) * a + t * b;
    }

}

