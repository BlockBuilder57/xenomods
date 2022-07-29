#pragma once

#include <bf2mods/prettyprinter.hpp>
#include <bf2mods/utils.hpp>
#include <cmath>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace mm {

	struct Pnt {
		int x;
		int y;
	};

	BF2MODS_CONVERTTO_TYPE(Vec2, glm::vec2, sizeof(glm::vec2));

	struct Vec3 : public bf2mods::ConvertTo<glm::vec3, sizeof(glm::vec3)> {
		// We actually want to add members to Vec3 so
		//BF2MODS_CONVERTTO_TYPE(Vec3, glm::vec3, sizeof(glm::vec3));
		// won't work. Ugly using, but oh well.
		using ConvertTo<glm::vec3, sizeof(glm::vec3)>::ConvertTo;

		inline float XZLengthSqu() const {
			glm::vec3 glm = *this;
			return std::sqrt(glm.x * glm.x + glm.z * glm.z);
		}

		inline void XZNormalizeInPlace() {
			glm::vec3 glm = *this;
			float length = glm::length(glm);
			glm.x /= length;
			glm.z /= length;
		}

		inline Vec3 XZNormalized() {
			Vec3 other = *this;
			other.XZNormalizeInPlace();
			return other;
		}
	};
	using Transform = Vec3;
	static_assert(sizeof(Vec3) == 0xC, "size 0xC");

	BF2MODS_CONVERTTO_TYPE(Quat, glm::quat, sizeof(glm::quat));

	BF2MODS_CONVERTTO_TYPE(alignas(0x10) Mat44, glm::mat4, sizeof(glm::mat4));

	static_assert(sizeof(Mat44) == 0x40, "[mm::Mat44] size 0x40");
	static_assert(alignof(mm::Mat44) == 0x10, "[mm::Mat44] align 0x10");

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

			glm::vec3 vec = vec3;
			ss << '(';
			PrintComponentWithName('X', vec.x);
			ss << ' ';
			PrintComponentWithName('Y', vec.y);
			ss << ' ';
			PrintComponentWithName('Z', vec.z);
			ss << ')';

			return ss.str();
		}

		inline static std::string_view type_name() {
			return "mm::Vec3";
		}
	};

	template<>
	struct Prettyprinter<mm::Mat44> {
		inline static std::string format(const mm::Mat44& mat44, const int precision = -1) {
			std::stringstream ss;

			if(precision > 0)
				ss.precision(precision);

			glm::mat4 mat;
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