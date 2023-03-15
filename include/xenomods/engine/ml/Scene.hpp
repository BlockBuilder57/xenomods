//
// Created by block on 6/26/2022.
//

#pragma once

#include "xenomods/Utils.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"

namespace ml {

	struct AttrTransform {
		mm::Vec3 pos;
		INSERT_PADDING_BYTES(0x4);
		mm::Vec3 camPos;
		INSERT_PADDING_BYTES(0x4);
		INSERT_PADDING_BYTES(sizeof(mm::Vec3));
		INSERT_PADDING_BYTES(0x4);
		INSERT_PADDING_BYTES(sizeof(mm::Mat44));
		mm::Mat44 viewMatInverse;
		mm::Mat44 viewMat;
		mm::Vec3 eulerAngles;
		INSERT_PADDING_BYTES(0x4);
		mm::Vec3 camRot;
		INSERT_PADDING_BYTES(0x4);
		INSERT_PADDING_BYTES(sizeof(mm::Vec3));
		INSERT_PADDING_BYTES(0x4);
		INSERT_PADDING_BYTES(sizeof(mm::Vec3));
		INSERT_PADDING_BYTES(0x4);
		INSERT_PADDING_BYTES(sizeof(mm::Vec3));
		INSERT_PADDING_BYTES(0x4);
		mm::Vec3 target;
		INSERT_PADDING_BYTES(0x4);
		INSERT_PADDING_BYTES(sizeof(mm::Vec3));
		INSERT_PADDING_BYTES(0x4);
		mm::Mat44 frustum;
		mm::Mat44 projection;
		mm::Pnt<int> camSize;
		float fov;
		float field_492_0x1ec;
		float camAspect;
		float zNear;
		float zFar;
	};

	class Scn;

	class ScnObjCam {
	   public:
		INSERT_PADDING_BYTES(0x8)
		Scn* ScnPtr {};
		INSERT_PADDING_BYTES(0x70)
		AttrTransform* AttrTransformPtr {};

		void setWorldMatrix(const mm::Mat44& matrix);
		void setViewMatrix(const mm::Mat44& matrix);
		void updateFovNearFar();
	};

	enum class SCNCAM : std::uint32_t {};
	enum class SCNPRIO_CB : std::uint32_t {};

	class Scn {
	   public:
		INSERT_PADDING_BYTES(0x38)
		void* somePointer;
		// will this work? lol
		ScnObjCam* getCam(int index);

		void setDebDraw(SCNCAM cam, SCNPRIO_CB prio);
		bool enableScnDebug(bool nuts);
	};

} // namespace ml
