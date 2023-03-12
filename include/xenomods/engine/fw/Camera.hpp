// Created by block on 2/26/2023.

#pragma once

#include <xenomods/engine/mm/MathTypes.hpp>
#include <xenomods/engine/mm/mtl/RTTI.hpp>

#include "Document.hpp"

namespace fw {

	class Camera {
	   public:
#if XENOMODS_CODENAME(bfsw)
		void* field1_0x8;
		void* field2_0x10;
		void* field3_0x18;
		unsigned int CAMERA_PRIO;
		unsigned int unk1;
		unsigned int unk2;
		unsigned int unk3;
		mm::Mat44 matrix;
		mm::Mat44 matrix2;
		float unk;
		float fov;
		float zNear;
		float zFar;
#else
		int m_RefCount;
		int pad;
		void* field1_0x10;
		void* field3_0x18;
		void* field4_0x20;
		unsigned int CAMERA_PRIO;
		float fov;
		float zNear;
		float zFar;
		INSERT_PADDING_BYTES(0x8);
		mm::Vec3 position;
		INSERT_PADDING_BYTES(0x4);
		mm::Vec3 lookAt;
		INSERT_PADDING_BYTES(0x4);
		mm::Mat44 matrix;
		bool activeCam;
#endif

		static mm::mtl::RTTI m_rtti;

		void setPosRot(const mm::Vec3& pos, const mm::Quat& rot);

#if XENOMODS_CODENAME(bfsw)
		void update(const fw::Document& doc, const fw::UpdateInfo& updateInfo);
#else
		void update(const fw::UpdateInfo& updateInfo);
#endif

		void updateViewMatrix();
		void setLookAt(const mm::Vec3& pos, const mm::Vec3& target, float roll);

		//virtual void RefObjDtor();
		virtual ~Camera() = default;

		virtual mm::mtl::RTTI* getRTTI() const;
		virtual char* getName() const;
	};

	class CameraLayer {
	   public:
		INSERT_PADDING_BYTES(0xd0);

		void update(const fw::UpdateInfo& updateInfo);
	};

	class CameraManager {
	   public:
		INSERT_PADDING_BYTES(0x20);
		CameraLayer Layer0;
		CameraLayer Layer1;
		CameraLayer Layer2;
		CameraLayer Layer3;
		void* notTheCamera;

		void update(const fw::UpdateInfo& updateInfo);
	};

} // namespace fw