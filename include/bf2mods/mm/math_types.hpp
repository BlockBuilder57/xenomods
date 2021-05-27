#pragma once

namespace mm {

struct Pnt {
    int x;
    int y;
};

struct Vec3 {
    float x;
    float y;
    float z;

    static const Vec3 zero;

    inline Vec3& operator+=(const Vec3& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
    inline Vec3& operator-=(const Vec3& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }
    inline Vec3 operator+(const Vec3& rhs) const { return Vec3{x + rhs.x, y + rhs.y, z + rhs.z}; }
    inline Vec3 operator-(const Vec3& rhs) const { return Vec3{x - rhs.x, y - rhs.y, z - rhs.z}; }
    inline Vec3 operator*(const float& rhs) { return Vec3{x * rhs, y * rhs, z * rhs}; }
};
static_assert(sizeof(Vec3) == 0xC, "size 0xC");

struct Quat {
    float a;
    float b;
    float c;
    float d;

    static const Quat zero;
};

}  // namespace mm
