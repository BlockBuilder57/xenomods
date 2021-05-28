#pragma once

#include <bf2mods/prettyprinter.hpp>

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

  inline Vec3 &operator+=(const Vec3 &rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
  }
  inline Vec3 &operator-=(const Vec3 &rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
  }
  inline Vec3 operator+(const Vec3 &rhs) const {
    return Vec3{x + rhs.x, y + rhs.y, z + rhs.z};
  }
  inline Vec3 operator-(const Vec3 &rhs) const {
    return Vec3{x - rhs.x, y - rhs.y, z - rhs.z};
  }
  inline Vec3 operator*(const float &rhs) {
    return Vec3{x * rhs, y * rhs, z * rhs};
  }
};
static_assert(sizeof(Vec3) == 0xC, "size 0xC");

struct Quat {
  float a;
  float b;
  float c;
  float d;

  static const Quat zero;
};

/**
 * Float RGBA color.
 */
struct Col4 {
  float r;
  float g;
  float b;
  float a;
};

static Col4 Col4_White = {1.f, 1.f, 1.f, 1.f};
static Col4 Col4_Black = {0.f, 0.f, 0.f, 1.f};

static_assert(sizeof(Col4) == sizeof(float) * 4, "[mm::Col4] size wrong!");

} // namespace mm

namespace bf2mods {

/**
 * Prettyprinter specialization for mm::Vec3
 */
template <> struct prettyprinter<mm::Vec3> {
  inline static std::string format(const mm::Vec3 &vec3) {
    std::stringstream ss;

    auto PrintComponentWithName = [&ss](char c, const float &coord) {
      ss << c << ": " << coord;
    };

    // yes its jank
    // yes it works
    // so it sticks

    ss << '(';
    PrintComponentWithName('X', vec3.x);
    ss << ' ';
    PrintComponentWithName('Y', vec3.y);
    ss << ' ';
    PrintComponentWithName('Z', vec3.z);
    ss << ')';
    return ss.str();
  }

  inline static std::string_view type_name() { return "mm::Vec3"; }
};

} // namespace bf2mods