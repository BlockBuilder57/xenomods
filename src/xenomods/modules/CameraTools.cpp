//
// Created by block on 6/21/2022.
//

#include "CameraTools.hpp"

#include <skylaunch/hookng/Hooks.hpp>
#include <xenomods/DebugWrappers.hpp>
#include <xenomods/HidInput.hpp>
#include <xenomods/Logger.hpp>

#include "DebugStuff.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/mat4x4.hpp"
#include "xenomods/engine/apps/FrameworkLauncher.hpp"
#include "xenomods/engine/fw/UpdateInfo.hpp"
#include "xenomods/engine/game/MenuModelView.hpp"
#include "xenomods/engine/ml/Scene.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"
#include "xenomods/stuff/utils/util.hpp"

namespace {

	struct PilotCameraLayers : skylaunch::hook::Trampoline<PilotCameraLayers> {
#if XENOMODS_CODENAME(bfsw)
		static void Hook(fw::CameraLayer* this_pointer, const fw::Document& document, const fw::UpdateInfo& updateInfo) {
#else
		static void Hook(fw::CameraLayer* this_pointer, const fw::UpdateInfo& updateInfo) {
#endif
			if (reinterpret_cast<void*>(this_pointer->listCamera.head) != &this_pointer->listCamera) {
				size_t ptrOffset = 0x10; //offsetof(fw::Camera, next);
				auto realHead = reinterpret_cast<fw::Camera*>(reinterpret_cast<u8*>(this_pointer->listCamera.head) - ptrOffset);
				//xenomods::g_Logger->LogDebug("list @ {} - head == {}, count {}", reinterpret_cast<void*>(&this_pointer->listCamera), reinterpret_cast<void*>(this_pointer->listCamera.head), this_pointer->listCamera.count);

				while (true) {
					//xenomods::g_Logger->LogDebug("so no head? {} -> {}", reinterpret_cast<void*>(realHead), reinterpret_cast<void*>(realHead->next));
					//dbgutil::logMemory(realHead, sizeof(fw::Camera));

					if (realHead->getRTTI()->isKindOf(&fw::Camera::m_rtti)) {
						if (xenomods::CameraTools::Freecam.isOn) {
							realHead->matrix = glm::inverse(static_cast<const glm::mat4&>(xenomods::CameraTools::Freecam.matrix));;
							realHead->fov = xenomods::CameraTools::Freecam.fov;

							//fw::debug::drawCompareZ(false);
							//fw::debug::drawCamera(glm::inverse(static_cast<const glm::mat4&>(realHead->matrix)), mm::Col4::cyan);
							//fw::debug::drawCompareZ(true);

							//std::string namey = std::string(realHead->getName());
							//xenomods::g_Logger->ToastInfo("ball" + namey, "{} prio {} fov {:.2f}", namey, realHead->CAMERA_PRIO, realHead->fov);

							//fw::debug::drawFontFmtShadow3D(xenomods::CameraTools::Meta.pos, mm::Col4::white, "Camera: {} prio {}", namey, realHead->CAMERA_PRIO);
						}
					}

					if (realHead->next == nullptr || reinterpret_cast<void*>(realHead->next) == &this_pointer->listCamera)
						break;

					realHead = reinterpret_cast<fw::Camera*>(reinterpret_cast<u8*>(realHead->next) - ptrOffset);
				}
			}

			if (xenomods::CameraTools::Freecam.isOn) {
				this_pointer->willLerp = true;
				this_pointer->lerpProgress = 999.f;
				this_pointer->matTarget = xenomods::CameraTools::Freecam.matrix;
				this_pointer->matCurrent = xenomods::CameraTools::Freecam.matrix;
			}

#if XENOMODS_CODENAME(bfsw)
			Orig(this_pointer, document, updateInfo);
#else
			Orig(this_pointer, updateInfo);
#endif

			if (xenomods::CameraTools::Freecam.isOn) {
				this_pointer->objCam->AttrTransformPtr->fov = xenomods::CameraTools::Freecam.fov;
				this_pointer->objCam->updateFovNearFar();
			}
		}
	};

	struct CopyCurrentCameraState : skylaunch::hook::Trampoline<CopyCurrentCameraState> {
			static void Hook(ml::ScnObjCam* this_pointer) {
				Orig(this_pointer);

				if(this_pointer->ScnPtr != nullptr && this_pointer == this_pointer->ScnPtr->getCam(-1)) {
					if(!xenomods::CameraTools::Freecam.isOn) {
						// read state from current camera
						xenomods::CameraTools::Freecam.matrix = this_pointer->AttrTransformPtr->viewMatInverse;
						xenomods::CameraTools::Freecam.fov = this_pointer->AttrTransformPtr->fov;
					}
				}
			}
	};

}; // namespace

namespace xenomods {

	CameraTools::FreecamState CameraTools::Freecam = {
		.isOn = false,
		.matrix = glm::identity<glm::mat4>(),
		.fov = 40.f,
		.camSpeed = 1.f
	};

	CameraTools::FreecamMeta CameraTools::Meta = {};

	void CameraTools::DoFreeCameraMovement(glm::mat4& matrix, FreecamMeta* meta) {
		// controls:
		// Left stick: Y: forward/back, X: left/right
		// Right stick: XY: Look movement
		// LStick hold: Y: fov up/down
		// RStick hold: X: roll left/right

		// for future reference:
		//auto seconds = nn::os::GetSystemTick()/19200000.;

		using enum xenomods::Keybind;

		glm::vec3 pos {};
		glm::quat rot {};
		glm::vec3 scale {};
		glm::vec3 skew {};
		glm::vec4 perspective {};

		// decompose existing matrix
		glm::decompose(matrix, scale, rot, pos, skew, perspective);

		glm::vec2 lStick = GetPlayer(2)->stateCur.LAxis;
		glm::vec2 rStick = GetPlayer(2)->stateCur.RAxis;

		// deadzone
		if(glm::length(lStick) < 0.15f)
			lStick = glm::zero<glm::vec2>();
		if(glm::length(rStick) < 0.15f)
			rStick = glm::zero<glm::vec2>();

		// movement
		glm::vec3 move {};

		if(!GetPlayer(2)->InputHeld(FREECAM_FOVHOLD)) {
			move = { lStick.x, 0, -lStick.y };
			move = rot * move;		  // rotate movement to local space
			move *= Freecam.camSpeed; // multiply by cam speed
		}

		// rotation
		glm::vec3 look {};
		float lookMult = 3.f;

		// slow the camera down at lower fovs
		if(std::abs(Freecam.fov) < 45.f)
			lookMult *= Freecam.fov / 45.f;

		if(GetPlayer(2)->InputHeld(FREECAM_ROLLHOLD))
			look = { 0, 0, -rStick.x * 0.5f }; // only roll
		else
			look = { rStick.y * lookMult, -rStick.x * lookMult, 0 }; // pitch and yaw

		// yaw is in world space
		float yawDeg = glm::radians(look.y);
		glm::quat yawRot = glm::angleAxis(yawDeg, glm::vec3(0, 1, 0));

		// pitch is in local space
		float pitchDeg = glm::radians(look.x);
		glm::quat pitchRot = glm::angleAxis(pitchDeg, rot * glm::vec3(1, 0, 0));

		// roll is in local space
		float rollDeg = glm::radians(look.z);
		glm::quat rollRot = glm::angleAxis(rollDeg, rot * glm::vec3(0, 0, 1));

		// apply yaw and pitch
		rot = yawRot * pitchRot * rollRot * rot;

		// get angle+axis to rotate the matrix by
		float angle = glm::angle(rot);
		glm::vec3 axis = glm::axis(rot);

#if 0
		int yPos = 200;
		const int height = fw::debug::drawFontGetHeight();
		fw::debug::drawFontFmtShadow(0, yPos += height, mm::Col4::white, "- freecam debug -");
		fw::debug::drawFontFmtShadow(0, yPos += height, mm::Col4::white, "pos: {:1}", pos);
		fw::debug::drawFontFmtShadow(0, yPos += height, mm::Col4::white, "rot: {:1}", glm::degrees(glm::eulerAngles(rot)));
		fw::debug::drawFontFmtShadow(0, yPos += height, mm::Col4::white, "speed: {}", Freecam.camSpeed);
		fw::debug::drawFontFmtShadow(0, yPos += height, mm::Col4::white, "fov: {:.1f}", Freecam.fov);
#endif

		if(meta != nullptr) {
			meta->pos = pos;
			meta->rot = rot;

			glm::vec3 forward = mm::Vec3::unitZ;
			glm::vec3 up = mm::Vec3::unitY;
			meta->forward = rot * forward;
			meta->up = rot * up;
		}

		glm::mat4 newmat = glm::mat4(1.f);
		newmat = glm::translate(newmat, pos + move);
		newmat = glm::rotate(newmat, angle, axis);

		matrix = newmat;
	}

	void CameraTools::Initialize() {
		g_Logger->LogDebug("Setting up camera tools...");

#if !XENOMODS_CODENAME(bfsw)
		// intermittently reads the address as 0x0... let's just use the actual symbol for now
		// TODO: why *is* the function reference not exporting?
		PilotCameraLayers::HookAt("_ZN2fw11CameraLayer6updateERKNS_10UpdateInfoE");
#else
		PilotCameraLayers::HookAt(&fw::CameraLayer::update);
#endif
		CopyCurrentCameraState::HookAt(&ml::ScnObjCam::updateFovNearFar);
	}

	void CameraTools::Update() {
		if(GetPlayer(2)->InputDownStrict(Keybind::FREECAM_TOGGLE)) {
			Freecam.isOn = !Freecam.isOn;
			g_Logger->ToastInfo(STRINGIFY(Freecam.isOn), "Freecam: {}", Freecam.isOn);
		}

		if(Freecam.isOn) {
			static float lastFOVPress = MAXFLOAT;
			static float lastRollPress = MAXFLOAT;
			float now = nn::os::GetSystemTick() / 19200000.;

			bool speedChanged = false;
			if(GetPlayer(2)->InputDownStrict(Keybind::FREECAM_SPEED_UP)) {
				Freecam.camSpeed *= 2.f;
				speedChanged = true;
			} else if(GetPlayer(2)->InputDownStrict(Keybind::FREECAM_SPEED_DOWN)) {
				Freecam.camSpeed /= 2.f;
				speedChanged = true;
			}

			if(speedChanged)
				g_Logger->ToastInfo("freecamSpeed", "Freecam speed: {}", Freecam.camSpeed);

			if(GetPlayer(2)->InputHeldStrict(Keybind::FREECAM_FOVHOLD)) {
				// holding down the button, so modify fov
				// note: game hard crashes during rendering when |fov| >= ~179.5, it needs clamping
				Freecam.fov = std::clamp(Freecam.fov + -GetPlayer(2)->stateCur.LAxis.y * 0.25f, -179.f, 179.f);
			}

			if(GetPlayer(2)->InputDownStrict(Keybind::FREECAM_FOVHOLD)) {
				if(std::abs(now - lastFOVPress) < 0.2f)
					Freecam.fov = 80;

				lastFOVPress = now;
			}
			if(GetPlayer(2)->InputDownStrict(Keybind::FREECAM_ROLLHOLD)) {
				if(std::abs(now - lastRollPress) < 0.2f) {
					glm::vec3 pos {};
					glm::quat rot {};
					glm::vec3 scale {};
					glm::vec3 skew {};
					glm::vec4 perspective {};

					// decompose existing matrix
					glm::decompose(static_cast<const glm::mat4&>(Freecam.matrix), scale, rot, pos, skew, perspective);
					glm::mat4 newmat = glm::identity<glm::mat4>();
					newmat = glm::translate(newmat, pos);
					// just don't apply any rotation

					Freecam.matrix = newmat;
				}

				lastRollPress = now;
			}
		}

		if(Freecam.isOn)
			DoFreeCameraMovement(static_cast<glm::mat4&>(Freecam.matrix), &Meta);
	}

	XENOMODS_REGISTER_MODULE(CameraTools);

} // namespace xenomods