//
// Created by block on 6/26/2022.
//

#pragma once

#include "../mm/math_types.hpp"
#include "../utils.hpp"

namespace ml {

	struct AttrTransform {
		mm::Vec3 pos;							//0x4
		INSERT_PADDING_BYTES(0x4)				//0xc
		INSERT_PADDING_BYTES(sizeof(mm::Vec3)); //0x10
		INSERT_PADDING_BYTES(0x4)				//0x1c
		INSERT_PADDING_BYTES(0x50)				//0x20
		mm::Mat44 CamInverseMatrix;				//0x70
		mm::Mat44 weirdMatrix;					//0xb0
		mm::Vec3 eulerAngles;					//0xf0
	};

	// This seems to be a struct that continues AttrTransform
	// (the pointer to it in ScnObjCam is a AttrTransform*
	// I would define it as such, but I've not no clue as to what
	// the true length is.
	struct AttrTransformInheriter {
		INSERT_PADDING_BYTES(0x1e8)
		float fov;
	};

	class Scn;

	class ScnObjCam {
	   public:
		INSERT_PADDING_BYTES(0x8)
		Scn* ScnPtr {};
		INSERT_PADDING_BYTES(0x70)
		AttrTransformInheriter* AttrTransformPtr {};
	};

	class Scn {
	   public:
		// will this work? lol
		ScnObjCam* getCam(int index);
	};

} // namespace ml
