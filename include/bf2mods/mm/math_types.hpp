#pragma once

#include <bf2mods/prettyprinter.hpp>
#include <cmath>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace mm {

	struct Pnt {
		int x;
		int y;
	};

	using Vec2 = glm::vec2;
	using Vec3 = glm::vec3;
	using Transform = Vec3;
	static_assert(sizeof(Vec3) == 0xC, "size 0xC");

	constexpr float Vec3XZLength(const Vec3& src) {
		return std::sqrt(src.x * src.x + src.z * src.z);
	}

	constexpr void Vec3XZNormalizeInPlace(Vec3& src) {
		float length = Vec3XZLength(src);
		src.x /= length;
		src.z /= length;
	}
	constexpr Vec3 Vec3XZNormalized(const Vec3& src) {
		Vec3 other = src;
		Vec3XZNormalizeInPlace(other);
		return other;
	}

	using Quat = glm::quat;

	using Mat44 = glm::mat4;
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
		inline static std::string format(const mm::Vec3& vec3, const int precision = -1) {
			std::stringstream ss;

			if(precision > 0)
				ss.precision(precision);

			auto PrintComponentWithName = [&ss](char c, const float& coord) {
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

		inline static std::string_view type_name() {
			return "mm::Vec3";
		}
	};

	template<>
	struct Prettyprinter<mm::Mat44> {
		inline static std::string format(const mm::Mat44& mat, const int precision = -1) {
			std::stringstream ss;

			if(precision > 0)
				ss.precision(precision);

			ss << '(';
			ss << mat[0][0] << ',';
			ss << mat[0][1] << ',';
			ss << mat[0][2] << ',';
			ss << mat[0][3] << ", ";
			ss << mat[1][0] << ',';
			ss << mat[1][1] << ',';
			ss << mat[1][2] << ',';
			ss << mat[1][3] << ", ";
			ss << mat[2][0] << ',';
			ss << mat[2][1] << ',';
			ss << mat[2][2] << ',';
			ss << mat[2][3] << ", ";
			ss << mat[3][0] << ',';
			ss << mat[3][1] << ',';
			ss << mat[3][2] << ',';
			ss << mat[3][3];
			ss << ')';

			return ss.str();
		}

		inline static std::string_view type_name() {
			return "mm::Mat44";
		}
	};

} // namespace bf2mods