// Created by block on 2/26/2023.

#pragma once

#include <xenomods/engine/mm/MathTypes.hpp>
#include <xenomods/engine/mm/mtl/RTTI.hpp>
#include <xenomods/engine/mm/mtl/IntrusiveList.hpp>

#include "Document.hpp"

namespace fw {

	class Camera {
	   public:
#if XENOMODS_CODENAME(bfsw) || XENOMODS_CODENAME(bf3)
		int m_RefCount;
		int pad;
		mm::mtl::IntrusiveListNodeBase* next;
		mm::mtl::IntrusiveListNodeBase* prev;
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
		mm::mtl::IntrusiveListNodeBase* next;
		mm::mtl::IntrusiveListNodeBase* prev;
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
#if XENOMODS_CODENAME(bf3)
		mm::mtl::IntrusiveListBase listCamera;
		mm::mtl::IntrusiveListBase listCameraPlugin;
		INSERT_PADDING_BYTES(0x98);
		ml::ScnObjCam* objCam;
		INSERT_PADDING_BYTES(0x48);
		mm::Mat44 matCurrent;
		mm::Mat44 matTarget;
		float unknown;
		float lerpProgress;
		float lerpTime;
		bool willLerp;
#else
		mm::mtl::IntrusiveListBase listCamera;
		mm::mtl::IntrusiveListBase listCameraPlugin;
		ml::ScnObjCam* objCam;
#if XENOMODS_CODENAME(bfsw)
		INSERT_PADDING_BYTES(0x8);
		mm::Mat44 matCurrent;
		mm::Mat44 matTarget;
		float unknown;
		float lerpProgress;
		float lerpTime;
		bool willLerp;
#else
		mm::Mat44 matCurrent;
		mm::Mat44 matTarget;
		float lerpProgress;
		float lerpTime;
		bool willLerp;
#endif
#endif

		void getCameraPosition() const;

#if XENOMODS_CODENAME(bfsw)
		void update(const fw::Document& document, const fw::UpdateInfo& updateInfo);
#else
		virtual void update(const fw::UpdateInfo& updateInfo);
#endif
	};

	class CameraManager {
	   public:
#if XENOMODS_CODENAME(bfsw)
		INSERT_PADDING_BYTES(0x10);
		CameraLayer Layers[8];
#else
		INSERT_PADDING_BYTES(0x20);
		CameraLayer Layers[4];
#endif
		fw::Camera* notTheCamera;

#if XENOMODS_CODENAME(bfsw)
		void update(const fw::Document& document, const fw::UpdateInfo& updateInfo);
#else
		virtual void update(const fw::UpdateInfo& updateInfo);
#endif
	};

} // namespace fw