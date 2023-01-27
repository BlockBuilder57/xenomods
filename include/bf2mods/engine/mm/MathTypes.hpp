#pragma once

#include <cmath>

#include "bf2mods/Utils.hpp"
#include "fmt/format.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

namespace mm {

	template<class T>
	struct Pnt {
		T x, y;
	};

	template<class T>
	struct Rect {
		T x, y, w, h;
	};

	BF2MODS_CONVERTTO_TYPE(Vec2, glm::vec2, sizeof(glm::vec2));

	struct Vec3 : public bf2mods::ConvertTo<glm::vec3, sizeof(glm::vec3)> {
		// We actually want to add members to Vec3 so
		//BF2MODS_CONVERTTO_TYPE(Vec3, glm::vec3, sizeof(glm::vec3));
		// won't work. Ugly using, but oh well.
		using ConvertTo<glm::vec3, sizeof(glm::vec3)>::ConvertTo;

		inline float XZLength() const {
			glm::vec3 glm = *this;
			return std::sqrt(glm.x * glm.x + glm.z * glm.z);
		}

		inline void XZNormalizeInPlace() {
			glm::vec3 glm = *this;
			float length = XZLength();
			glm.x /= length;
			glm.z /= length;
		}

		inline Vec3 XZNormalized() {
			Vec3 other = *this;
			other.XZNormalizeInPlace();
			return other;
		}

		static const Vec3 zero;
		static const Vec3 unitX;
		static const Vec3 unitY;
		static const Vec3 unitZ;
		static const Vec3 unit;
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

		static const Col4 white;   // 1.0, 1.0, 1.0, 1.0 (0xFFFFFFFF)
		static const Col4 gray;    // 0.5, 0.5, 0.5, 1.0 (0x7F7F7FFF)
		static const Col4 black;   // 0.0, 0.0, 0.0, 1.0 (0x000000FF)
		static const Col4 red;     // 1.0, 0.0, 0.0, 1.0 (0xFF0000FF)
		static const Col4 green;   // 0.0, 1.0, 0.0, 1.0 (0x00FF00FF)
		static const Col4 blue;    // 0.0, 0.0, 1.0, 1.0 (0x0000FFFF)
		static const Col4 yellow;  // 1.0, 1.0, 0.0, 1.0 (0xFFFF00FF)
		static const Col4 cyan;    // 0.0, 1.0, 1.0, 1.0 (0x00FFFFFF)
		static const Col4 magenta; // 1.0, 0.0, 1.0, 1.0 (0xFF00FFFF)
		static const Col4 salmon;  // 1.0, 0.5, 0.5, 1.0 (0xFF7FFFFF)
		static const Col4 orange;  // 1.0, 0.5, 0.0, 1.0 (0xFF7F00FF)
		static const Col4 zero;    // 0.0, 0.0, 0.0, 0.0 (0x00000000)

		mm::Col4 operator*(const float& rhs) {
			mm::Col4 color;
			memcpy(&color, this, sizeof(mm::Col4));

			// color ain't linear, folks!
			float cubicEaseOut = 1.f - powf(1.f - rhs, 3.f);
			color.r *= cubicEaseOut;
			color.g *= cubicEaseOut;
			color.b *= cubicEaseOut;
			return color;
		}

		void operator*=(const float& rhs) {
			// color ain't linear, folks!
			float cubicEaseOut = 1.f - powf(1.f - rhs, 3.f);
			this->r *= cubicEaseOut;
			this->g *= cubicEaseOut;
			this->b *= cubicEaseOut;
		}
	};

	static_assert(sizeof(Col4) == sizeof(float) * 4, "[mm::Col4] size wrong!");

} // namespace mm

/**
 * Formatter specializations for math types
 *
 * README IF CRASHING: this isn't a float formatter!
 * If you're using {:.Nf}, don't! Use {:N}!
 */

template<>
struct fmt::formatter<glm::vec2> : fmt::formatter<std::string> {
	int precision = 0;

	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		auto it = ctx.begin(), end = ctx.end();
		if(it != end && isdigit(*it)) {
			auto p = *it++;

			// cheap hack to make it a number, this means that
			// any number over 9 won't work (but this is practical enough)
			precision = p - '0';
		} else {
			// this SHOULD be an error
			//ctx.on_error("invalid format");
		}

		// Check if reached the end of the range:
		if(it != end && *it != '}')
			ctx.on_error("invalid format");

		// Return an iterator past the end of the parsed range:
		return it;
	}

	template<typename FormatContext>
	inline auto format(const glm::vec2& glmVec, FormatContext& ctx) {
		return fmt::format_to(ctx.out(), FMT_STRING("(X: {1:.{0}f}, Y: {2:.{0}f})"), precision, glmVec.x, glmVec.y);
	}
};

template<>
struct fmt::formatter<glm::vec3> : fmt::formatter<std::string> {
	int precision = 0;

	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		auto it = ctx.begin(), end = ctx.end();
		if(it != end && isdigit(*it)) {
			auto p = *it++;

			// cheap hack to make it a number, this means that
			// any number over 9 won't work (but this is practical enough)
			precision = p - '0';
		} else {
			// this SHOULD be an error
			//ctx.on_error("invalid format");
		}

		// Check if reached the end of the range:
		if(it != end && *it != '}')
			ctx.on_error("invalid format");

		// Return an iterator past the end of the parsed range:
		return it;
	}

	template<typename FormatContext>
	inline auto format(const glm::vec3& glmVec, FormatContext& ctx) {
		return fmt::format_to(ctx.out(), FMT_STRING("(X: {1:.{0}f}, Y: {2:.{0}f}, Z: {3:.{0}f})"), precision, glmVec.x, glmVec.y, glmVec.z);
	}
};

template<>
struct fmt::formatter<glm::mat4> : fmt::formatter<std::string> {
	int precision = 0;

	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		auto it = ctx.begin(), end = ctx.end();
		if(it != end && isdigit(*it)) {
			auto p = *it++;

			// cheap hack to make it a number, this means that
			// any number over 9 won't work (but this is practical enough)
			precision = p - '0';
		} else {
			// this SHOULD be an error
			//ctx.on_error("invalid format");
		}

		// Check if reached the end of the range:
		if(it != end && *it != '}')
			ctx.on_error("invalid format");

		// Return an iterator past the end of the parsed range:
		return it;
	}

	template<typename FormatContext>
	inline auto format(const glm::mat4& glmMat, FormatContext& ctx) {
		// I don't get paid enough to do this - lily
		return fmt::format_to(ctx.out(),
							  FMT_STRING("( {1:.{0}f} {2:.{0}f} {3:.{0}f} {4:.{0}f},"
										 " {5:.{0}f} {6:.{0}f} {7:.{0}f} {8:.{0}f},"
										 " {9:.{0}f} {10:.{0}f} {11:.{0}f} {12:.{0}f},"
										 " {13:.{0}f} {14:.{0}f} {15:.{0}f} {16:.{0}f} )"),
							  precision,
							  glmMat[0][0], glmMat[0][1], glmMat[0][2], glmMat[0][3],
							  glmMat[1][0], glmMat[1][1], glmMat[1][2], glmMat[1][3],
							  glmMat[2][0], glmMat[2][1], glmMat[2][2], glmMat[2][3],
							  glmMat[3][0], glmMat[3][1], glmMat[3][2], glmMat[3][3]);
	}
};

template<>
struct fmt::formatter<glm::quat> : fmt::formatter<std::string> {
	int precision = 0;

	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		auto it = ctx.begin(), end = ctx.end();
		if(it != end && isdigit(*it)) {
			auto p = *it++;

			// cheap hack to make it a number, this means that
			// any number over 9 won't work (but this is practical enough)
			precision = p - '0';
		} else {
			// this SHOULD be an error
			//ctx.on_error("invalid format");
		}

		// Check if reached the end of the range:
		if(it != end && *it != '}')
			ctx.on_error("invalid format");

		// Return an iterator past the end of the parsed range:
		return it;
	}

	template<typename FormatContext>
	inline auto format(const glm::quat& glmQuat, FormatContext& ctx) {
		return fmt::format_to(ctx.out(), FMT_STRING("(X: {1:.{0}f}, Y: {2:.{0}f}, Z: {3:.{0}f}, W: {4:.{0}f})"), precision, glmQuat.x, glmQuat.y, glmQuat.z, glmQuat.w);
	}
};

#if 0
// Adapters to autocast from our ConvertTo<T, ...> types to
// the GLM underlying types, reusing the GLM formatter code

template<>
struct fmt::formatter<mm::Vec3> : fmt::formatter<glm::vec3> {
	template<typename FormatContext>
	inline auto format(const mm::Vec3& vec, FormatContext& ctx) {
		return fmt::format_to(ctx.out(), "{1:{0}}", precision, static_cast<const glm::vec3&>(vec));
	}
};

template<>
struct fmt::formatter<mm::Mat44> : fmt::formatter<glm::mat4> {
	template<typename FormatContext>
	inline auto format(const mm::Mat44& mat, FormatContext& ctx) {
		return fmt::format_to(ctx.out(), "{1:{0}}", precision, static_cast<const glm::mat4&>(mat));
	}
};
#endif