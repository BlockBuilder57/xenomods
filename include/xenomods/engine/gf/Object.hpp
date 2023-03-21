// Created by block on 3/19/23.

#pragma once

#include "xenomods/engine/mm/MathTypes.hpp"

namespace gf {

	struct GF_OBJ_HANDLE {
	   public:
		void* ptr;
	};

	struct GfComTransform {
	   public:
		mm::Vec3* getPosition() const;
		mm::Quat* getRotation() const;

		void setPosition(const mm::Vec3& pos);
		void setRotation(const mm::Quat& rot);
	};

	class GfObjAcc {
	   public:
		GfObjAcc(GF_OBJ_HANDLE* handle);

		bool getObjPosRot(mm::Vec3& pos, float& rot);
	};

} // namespace gf