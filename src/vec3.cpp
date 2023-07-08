#include "vec3.h"
#include <array>
#include <cmath>

namespace proxima {
    float deg2rad(float degree) {
        return degree / 180 * M_PI;
    }

    Mat4::Mat4() : Mat4({{
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }}) {}

    Mat4::Mat4(std::array<std::array<float, 4>, 4> rows) {
        this->_rows = rows;
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

    Vec3 operator*(Mat4 a, const Vec3 &v) {
        float elements[4];
        for (int i=0; i<4; i++)
            elements[i] = a[i][0] * v.x + a[i][1] * v.y + a[i][2] * v.z + a[i][3];
        return Vec3(elements[0], elements[1], elements[2]) / elements[3];
    }

    Mat4 operator*(Mat4 a, Mat4 b) {
        Mat4 result;
        for (int i=0; i<4; i++) {
            for (int j=0; j<4; j++) {
                result[i][j] = 0;
                for (int k=0; k<4; k++) {
                    result[i][j] += a[i][k] * b[k][j];
                }
            }
        }
        return result;
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

    Vec3 lerp(Vec3 a, Vec3 b, float t) {
        return (1 - t) * a + t * b;
    }

    Vec3 rotate(Vec3 v, Vec3 eulers) {
        float x = deg2rad(eulers.x);
        float y = deg2rad(eulers.y);
        float z = deg2rad(eulers.z);
        return Mat4::RotY(y) * Mat4::RotX(x) * Mat4::RotZ(z) * v;
    }
}

