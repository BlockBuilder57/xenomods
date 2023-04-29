// Created by block on 3/19/23.

#pragma once

#include "xenomods/engine/mm/MathTypes.hpp"

namespace gf {

	struct GF_OBJ_HANDLE {
	   public:
		unsigned long actual;

		// xenomods from here
		inline bool IsValid() {
			return actual != -1ul;
		}
		inline GF_OBJ_HANDLE* Ptr() {
			return reinterpret_cast<GF_OBJ_HANDLE*>(actual);
		}
	};

	struct GfComTransform {
	   public:
		mm::Vec3* getPosition() const;
		mm::Quat* getRotation() const;

		void setPosition(const mm::Vec3& pos);
		void setRotation(const mm::Quat& rot);
	};

	// unknown name
	enum class ObjectType {
		Type1,
		Type2,
		Type3,
		Type4,
		Type5,
		Type6,
		Type7,
		Type8
	};

	class GfObjAcc {
	   public:
		GfObjAcc(GF_OBJ_HANDLE* handle);

		ObjectType getType() const;

		bool getObjPosRot(mm::Vec3& pos, float& rot);
	};

	class GfObjUtil {
	   public:
		static void* getProperty(GF_OBJ_HANDLE* handle);
	};

} // namespace gf