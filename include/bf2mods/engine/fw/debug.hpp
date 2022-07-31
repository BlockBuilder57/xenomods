//
// Created by block on 7/21/2022.
//

#pragma once

#include "bf2mods/engine/mm/math_types.hpp"

namespace fw {
	namespace debug {

		bool drawAxis(const mm::Mat44& transform, float scale);
		bool drawBox(const mm::Mat44& transform, const mm::Vec3& size, const mm::Col4& color);
		bool drawBoxLine(const mm::Mat44& transform, const mm::Vec3& size, const mm::Col4& color);
		bool drawCamera(const mm::Mat44& transform, const mm::Col4& color);
		bool drawCone(const mm::Mat44& transform, float height, float radius, const mm::Col4& color, unsigned int noClue);
		bool drawFont(int x, int y, const mm::Col4& color, const char* fmt, ...);
		int drawFontGetWidth(const char* fmt, ...);
		int drawFontGetHeight();
		void drawCompareZ(bool compare);

		unsigned drawArrow(const mm::Vec3& vStart, const mm::Vec3& vEnd, const mm::Col4& color);
		unsigned drawLine(const mm::Vec3& vStart, const mm::Vec3& vEnd, const mm::Col4& color);

	} // namespace debug

	namespace PadManager {
		void enableDebugDraw(bool enable);
	}
} // namespace fw
