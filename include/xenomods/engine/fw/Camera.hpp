// Created by block on 2/26/2023.

#pragma once

#include <xenomods/engine/mm/MathTypes.hpp>
#include <xenomods/engine/mm/mtl/RTTI.hpp>

#include "Document.hpp"

namespace fw {

	class Camera {
	   public:
		void* field1_0x8;
		void* field2_0x10;
		void* field3_0x18;
		unsigned int CAMERA_PRIO;
		unsigned int unk1;
		unsigned int unk2;
		unsigned int unk3;
		mm::Mat44 Matrix1;
		mm::Mat44 Matrix2;
		float unk;
		float fov;
		float zNear;
		float zFar;

		static mm::mtl::RTTI m_rtti;

		void setPosRot(const mm::Vec3& pos, const mm::Quat& rot);

#if XENOMODS_CODENAME(bfsw)
		void update(const fw::Document& doc, const fw::UpdateInfo& updateInfo);
#else
		void update(const fw::UpdateInfo& updateInfo);
#endif

		//virtual void RefObjDtor();
		virtual ~Camera() = default;

		virtual mm::mtl::RTTI* getRTTI() const;
		virtual char* getName() const;
	};

} // namespace fw