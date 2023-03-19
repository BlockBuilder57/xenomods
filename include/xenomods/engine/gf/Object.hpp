// Created by block on 3/19/23.

#pragma once

#include "xenomods/engine/mm/MathTypes.hpp"

namespace gf {

	struct GF_OBJ_HANDLE {
	   public:
		void* ptr;
	};

	class GfObjAcc {
	   public:
		GfObjAcc(GF_OBJ_HANDLE* handle);

		bool getObjPosRot(mm::Vec3& pos, float& rot);
	};

} // namespace gf