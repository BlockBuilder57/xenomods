#pragma once

#include <bf2mods/prettyprinter.hpp>
#include <cmath>

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

		inline Vec3 &operator*=(const Vec3 &rhs) {
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			return *this;
		}

		inline Vec3 &operator*=(const float &rhs) {
			x *= rhs;
			y *= rhs;
			z *= rhs;
			return *this;
		}

		inline float LengthSqu() const { return std::sqrt(x * x + y * y + z * z); }
		inline float XZLengthSqu() const { return std::sqrt(x * x + z * z); }

		inline void NormalizeInPlace() {
			float length = LengthSqu();
			x /= length;
			y /= length;
			z /= length;
		}
		inline void XZNormalizeInPlace() {
			float length = LengthSqu();
			x /= length;
			z /= length;
		}

		/**
		 * Returns a new Vec3 which is normalized.
		 */
		inline Vec3 Normalized() {
			Vec3 other = *this;
			other.NormalizeInPlace();
			return other;
		}
		inline Vec3 XZNormalized() {
			Vec3 other = *this;
			other.XZNormalizeInPlace();
			return other;
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

	struct Mat44 {
		float M11, M12, M13, M14;
		float M21, M22, M23, M24;
		float M31, M32, M33, M34;
		float M41, M42, M43, M44;

		static const Mat44 identity;
	};
	static_assert(sizeof(Mat44) == 0x40, "[mm::Mat44] size 0x40");

	/**
	 * Float RGBA color.
	 */
	struct Col4 {
		float r;
		float g;
		float b;
		float a;

		static const Col4 White;
		static const Col4 Black;
	};

	static_assert(sizeof(Col4) == sizeof(float) * 4, "[mm::Col4] size wrong!");

} // namespace mm

namespace bf2mods {

	/**
	 * Prettyprinter specialization for mm::Vec3
	 */
	template<>
	struct Prettyprinter<mm::Vec3> {
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