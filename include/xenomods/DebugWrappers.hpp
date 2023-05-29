//
// Created by block on 7/30/2022.
//

#pragma once

#include <skylaunch/hookng/Hooks.hpp>

#include "xenomods/engine/fw/Debug.hpp"
#include "xenomods/engine/gf/Util.hpp"
#include "xenomods/engine/ml/DebugDrawing.hpp"

namespace xenomods::debug {

	inline static mm::Col4 drawFontCurBackColor = {};

	inline void drawFontBackColor(const mm::Col4& color) {
		drawFontCurBackColor = color;

#if XENOMODS_CODENAME(bf3)
		auto debDraw = ((ml::DebDraw*(*)(int))(skylaunch::utils::g_MainTextAddr + 0x1243890))(-1); // ml::DebDraw::get
		((void(*)(ml::CacheDraw*, const mm::Col4&))(skylaunch::utils::g_MainTextAddr + 0x12450d8))(debDraw->pCacheDraw, color); // ml::CacheDraw::fontBack
#else
		auto cacheDraw = ml::DebDraw::getCacheDraw();
		cacheDraw->fontBack(color);
#endif
	}

	template<class... Args>
	inline int drawFontGetWidth(const char* fmt, Args... args) {
#if XENOMODS_CODENAME(bf3)
		// a bit of a travesty here...
		static char buf[512];
		snprintf(&buf[0], 512, fmt, std::forward<Args>(args)...);
		// FIXME: find a proper function to address this hardcoded nonsense
		// also why is it 8 and not 16?
		return 8 * strlen(buf);
#else
		return fw::debug::drawFontGetWidth(fmt, std::forward<Args>(args)...);
#endif
	}

	inline int drawFontGetHeight() {
#if XENOMODS_CODENAME(bf3)
		return 15;
#else
		static Lazy<int> lazy([]() {
			return fw::debug::drawFontGetHeight();
		});
		return lazy();
#endif
	}

	template<class... Args>
	void drawFont(int x, int y, mm::Col4 color, const char* fmt, Args... args) {
#if XENOMODS_CODENAME(bf3)
		// a bit of a travesty here...
		static char buf[512];
		snprintf(&buf[0], 512, fmt, std::forward<Args>(args)...);
		auto debDraw = ((ml::DebDraw*(*)(int))(skylaunch::utils::g_MainTextAddr + 0x1243890))(-1); // ml::DebDraw::get
		((void(*)(ml::CacheDraw*, const mm::Col4&))(skylaunch::utils::g_MainTextAddr + 0x1244f5c))(debDraw->pCacheDraw, color); // ml::CacheDraw::fontColor
		((void(*)(ml::CacheDraw*, short, short, const char*))(skylaunch::utils::g_MainTextAddr + 0x12456c8))(debDraw->pCacheDraw, x, y, buf); // ml::CacheDraw::font
#else
		fw::debug::drawFont(x, y, color, fmt, std::forward<Args>(args)...);
#endif
	}

	template<class... Args>
	void drawFontShadow(int x, int y, mm::Col4 color, const char* fmt, Args... args) {
		mm::Col4 backCol = drawFontCurBackColor;

		// back colors will draw twice unless we disable them for the shadow
		if (backCol.a > 0)
			drawFontBackColor({});

		drawFont(x + 1, y + 1, color * 0.1f, fmt, std::forward<Args>(args)...);

		// can renable if needed
		if (backCol.a > 0)
			drawFontBackColor(backCol);

		drawFont(x, y, color, fmt, std::forward<Args>(args)...);
	}

	template<class FormatString, typename... Args>
	inline int drawFontFmtGetWidth(const FormatString& format, Args&&... args) {
		std::string formatted = fmt::vformat(format, fmt::make_format_args(args...));
		return drawFontGetWidth("%s", formatted.c_str());
	}

	template<class FormatString, typename... Args>
	inline void drawFontFmt(int x, int y, const mm::Col4& color, const FormatString& format, Args&&... args) {
		std::string formatted = fmt::vformat(format, fmt::make_format_args(args...));
		drawFont(x, y, color, "%s", formatted.c_str());
	}

	template<class FormatString, typename... Args>
	inline void drawFontFmtShadow(int x, int y, const mm::Col4& color, const FormatString& format, Args&&... args) {
		std::string formatted = fmt::vformat(format, fmt::make_format_args(args...));
		drawFontShadow(x, y, color, "%s", formatted.c_str());
	}

	template<class FormatString, typename... Args>
	inline void drawFontFmt3D(const mm::Vec3 pos, const mm::Col4& color, const FormatString& format, Args&&... args) {
		std::string formatted = fmt::vformat(format, fmt::make_format_args(args...));
		glm::vec3 screenPoint {};
		gf::util::getScreenPos(reinterpret_cast<mm::Vec3&>(screenPoint), pos);
		if (screenPoint.z > 0)
			drawFont(screenPoint.x, screenPoint.y, color, "%s", formatted.c_str());
	}

	template<class FormatString, typename... Args>
	inline void drawFontFmtShadow3D(const mm::Vec3 pos, const mm::Col4& color, const FormatString& format, Args&&... args) {
		std::string formatted = fmt::vformat(format, fmt::make_format_args(args...));
		glm::vec3 screenPoint {};
		gf::util::getScreenPos(reinterpret_cast<mm::Vec3&>(screenPoint), pos);
		if (screenPoint.z > 0)
			drawFontShadow(screenPoint.x, screenPoint.y, color, "%s", formatted.c_str());
	}

}
