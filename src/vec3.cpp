#include "vec3.h"
#include <array>
#include <cmath>

namespace proxima {
    float deg2rad(float degree) {
        return degree / 180 * M_PI;
    }

    Matrix3x3::Matrix3x3(std::array<Vec3, 3> rows) {
        for (int i=0; i<3; i++) {
            this->rows[i] = rows[i];
        }
    }

    float Vec3::norm() {
        return sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
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
        os << '{' << v.x << ", " << v.y << ", " << v.z << '}';
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

    Vec3 operator*(Matrix3x3 a, const Vec3 &v) {
        float x = dot(a.rows[0], v);
        float y = dot(a.rows[1], v);
        float z = dot(a.rows[2], v);
        return Vec3(x, y, z);
    }

    Vec3 rotate(Vec3 v, Vec3 eulers) {
        float x = deg2rad(eulers.x);
        float y = deg2rad(eulers.y);
        float z = deg2rad(eulers.z);
        Matrix3x3 mx({
            Vec3(1,      0,       0),
            Vec3(0, cos(x), -sin(x)),
            Vec3(0, sin(x),  cos(x))
        });
        Matrix3x3 my({
            Vec3( cos(y), 0, sin(y)),
            Vec3(      0, 1,      0),
            Vec3(-sin(y), 0, cos(y))
        });
        Matrix3x3 mz({
            Vec3(cos(z), -sin(z), 0),
            Vec3(sin(z),  cos(z), 0),
            Vec3(     0,       0, 1)
        });
        return my * (mx * (mz * v));
    }
}

