//
// Created by block on 7/21/2022.
//

#pragma once

#include "xenomods/engine/mm/MathTypes.hpp"

namespace fw {

	struct PrimCapsule {
		mm::Vec3 normal;
		INSERT_PADDING_BYTES(0x4);
		mm::Vec3 posStart;
		INSERT_PADDING_BYTES(0x4);
		mm::Vec3 posEnd;
		INSERT_PADDING_BYTES(0x4);
		float radius;

		void getCenter(mm::Vec3&) const;
		float getHeight() const;
		void getMat(mm::Mat44&) const;
		void getRot(mm::Quat&) const;

		void setup(const mm::Vec3& start, const mm::Vec3& end, float radius);
		void setup(const mm::Vec3& pos, const mm::Quat& rot, float height, float radius);
		void setup(const mm::Mat44& mat, float height, float radius);
	};

	struct PrimSphere {
		mm::Vec3 center;
		INSERT_PADDING_BYTES(0x4);
		float radius;

		void getCenter(mm::Vec3&) const;

		void setup(const mm::Vec3& pos, float radius);
	};

	namespace debug {

		bool drawArrow(const mm::Vec3& start, const mm::Vec3& end, const mm::Col4& color);

		bool drawAxis(const mm::Mat44& transform, float scale);

		bool drawBox(const mm::Mat44& transform, const mm::Vec3& size, const mm::Col4& color);
		bool drawBoxLine(const mm::Mat44& transform, const mm::Vec3& size, const mm::Col4& color);

		bool drawCamera(const mm::Mat44& transform, const mm::Col4& color);

		bool drawCapsule(const fw::PrimCapsule& cap, const mm::Col4& color, unsigned int segments);
		bool drawCapsule(const mm::Mat44& transform, float height, float radius, const mm::Col4& color, unsigned int segments);
		bool drawCapsuleLine(const fw::PrimCapsule& cap, const mm::Col4&, unsigned int segments);
		bool drawCapsuleLine(const mm::Mat44& transform, float height, float radius, const mm::Col4& color, unsigned int segments);

		bool drawCircle2D(const mm::Pnt<int>& pos, float radius, const mm::Col4& color, unsigned int segments, const mm::Mat44&);
		bool drawCircleLine(const mm::Mat44& transform, float radius, const mm::Col4& color);
		bool drawCircleLine2D(const mm::Pnt<int>& pos, float radius, const mm::Col4& color, unsigned int segments, const mm::Mat44&);

		void drawCompareZ(bool compare);

		bool drawCone(const mm::Mat44& transform, float height, float radius, const mm::Col4& color, unsigned int segments);
		bool drawConeLine(const mm::Mat44& transform, float height, float radius, const mm::Col4& color, unsigned int segments);

		bool drawCylinder(const mm::Mat44& transform, float height, float radius, const mm::Col4& color, unsigned int segments);
		bool drawCylinderLine(const mm::Mat44& transform, float height, float radius, const mm::Col4& color, unsigned int segments);

		bool drawFont(int x, int y, const char* format, ...);
		bool drawFont(int x, int y, const mm::Col4& color, const char* format, ...);
		int drawFontGetWidth(const char* format, ...);
		int drawFontGetHeight();
		bool drawFontScale(float x, float y);

		bool drawGrid(const mm::Vec3& pos, int linesPerAxis, float spacing, const mm::Col4& color);
		bool drawLine(const mm::Vec3& start, const mm::Vec3& end, const mm::Col4& color);
		bool drawLine2D(const mm::Pnt<int>& start, const mm::Pnt<int>& end, const mm::Col4& color);
		bool drawLineSquare2D(const mm::Pnt<int>& p1, const mm::Pnt<int>& p2, const mm::Col4& color);
		bool drawLineSquare2D(const mm::Rect<int>& rect, const mm::Col4& color);

		bool drawOvalLine(const mm::Mat44& transform, float height, float width, const mm::Col4& color);

		bool drawPlaneSphere(const mm::Mat44& transform, float radius, const mm::Col4& color, unsigned int segments);
		bool drawPlaneSphereLine(const mm::Mat44& transform, float radius, const mm::Col4& color, unsigned int segments);

		bool drawPoint2D(const mm::Pnt<int>& pos, float radius, const mm::Col4& color);

		bool drawSphere(const fw::PrimSphere& sph, const mm::Col4& color, unsigned int segments);
		bool drawSphere(const mm::Mat44& transform, float radius, const mm::Col4& color, unsigned int segments);
		bool drawSphereLine(const fw::PrimSphere& sph, const mm::Col4& color, unsigned int segments);
		bool drawSphereLine(const mm::Mat44& transform, float radius, const mm::Col4& color, unsigned int segments);

		bool drawSquare(const mm::Mat44& transform, float width, float height, const mm::Col4& color);
		bool drawSquare2D(const mm::Pnt<int>& p1, const mm::Pnt<int>& p2, const mm::Col4& color);
		bool drawSquare2D(const mm::Rect<int>& rect, const mm::Col4& color);
		bool drawSquareLine(const mm::Mat44& transform, float width, float height, const mm::Col4& color);

		bool drawTriangle(const mm::Mat44& transform, const mm::Vec3& p1, const mm::Vec3& p2, const mm::Vec3& p3, const mm::Col4& color);
		bool drawTriangle2D(const mm::Pnt<int>& p1, const mm::Pnt<int>& p2, const mm::Pnt<int>& p3, const mm::Col4& color);

	} // namespace debug

	namespace PadManager {
		void enableDebugDraw(bool enable);
	}
} // namespace fw
