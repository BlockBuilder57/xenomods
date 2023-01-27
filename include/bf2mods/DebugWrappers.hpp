//
// Created by block on 7/30/2022.
//

#pragma once

#include <bf2mods/engine/fw/Debug.hpp>
#include <bf2mods/engine/gf/Util.hpp>

namespace fw::debug {

	template<class... Args>
	void drawFontShadow(int x, int y, mm::Col4 color, const char* fmt, Args... args) {
		drawFont(x + 1, y + 1, color * 0.1f, fmt, std::forward<Args>(args)...);
		drawFont(x, y, color, fmt, std::forward<Args>(args)...);
	}

	template<class FormatString, typename... Args>
	inline void drawFontFmt(int x, int y, const mm::Col4& color, const FormatString& format, Args&&... args) {
		auto formatted = fmt::vformat(format, fmt::make_format_args(args...));
		drawFont(x, y, color, "%s", formatted.c_str());
	}

	template<class FormatString, typename... Args>
	inline void drawFontFmtShadow(int x, int y, const mm::Col4& color, const FormatString& format, Args&&... args) {
		auto formatted = fmt::vformat(format, fmt::make_format_args(args...));
		drawFontShadow(x, y, color, "%s", formatted.c_str());
	}

	template<class FormatString, typename... Args>
	inline void drawFontFmt3D(const mm::Vec3 pos, const mm::Col4& color, const FormatString& format, Args&&... args) {
		auto formatted = fmt::vformat(format, fmt::make_format_args(args...));
		glm::vec3 screenPoint {};
		gf::util::getScreenPos(reinterpret_cast<mm::Vec3&>(screenPoint), pos);
		if (screenPoint.z > 0)
			drawFont(screenPoint.x, screenPoint.y, color, "%s", formatted.c_str());
	}

	template<class FormatString, typename... Args>
	inline void drawFontFmtShadow3D(const mm::Vec3 pos, const mm::Col4& color, const FormatString& format, Args&&... args) {
		auto formatted = fmt::vformat(format, fmt::make_format_args(args...));
		glm::vec3 screenPoint {};
		gf::util::getScreenPos(reinterpret_cast<mm::Vec3&>(screenPoint), pos);
		if (screenPoint.z > 0)
			drawFontShadow(screenPoint.x, screenPoint.y, color, "%s", formatted.c_str());
	}

}
