// Created by block on 2/25/2023.

#pragma once

#include "xenomods/engine/fw/Camera.hpp"
#include "xenomods/engine/fw/Document.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"

#if XENOMODS_NEW_ENGINE
namespace game {

	class MenuModelViewCamera : public fw::Camera {
	   public:
		mm::Vec3 pos;
		float unk;
		mm::Vec3 rot;
		float unk2;
		mm::Vec3 lookAt;

		void updateLookAt();
	};

	struct MenuModelView_weirdCameraStruct {
		fw::Camera* camPtr;
		INSERT_PADDING_BYTES(0x20 - sizeof(void*));
	};

	class MenuModelView {
	   public:
		void* vtable;
		fw::Document* doc;

		INSERT_PADDING_BYTES(0x30);

		MenuModelView_weirdCameraStruct** cameraStruct;
		INSERT_PADDING_BYTES(280 - 64 - 8);
		unsigned long cameraStructCount;

		void setCameraFov(float fov, unsigned int camID = -1);
		void setCameraPos(const mm::Vec3& pos, unsigned int camID = -1);
		void setCameraAt(const mm::Vec3& pos, unsigned int camID = -1);
		void setCameraRot(const mm::Vec3& unk, unsigned int camID = -1);

		void update(const fw::UpdateInfo&);
	};

} // namespace game
#endif