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

	struct SetCameraMatrix : skylaunch::hook::Trampoline<SetCameraMatrix> {
		static void Hook(ml::ScnObjCam* this_pointer, mm::Mat44& matrix) {
			auto& freecam = xenomods::CameraTools::Freecam;
			mm::Col4 camColor = mm::Col4::white;

#if XENOMODS_CODENAME(bfsw)
			mm::Mat44 trueMatrix = matrix;
#else
			// remember: 2 and Torna camera matricies are not world-space
			// for these, the inverse of the matrix is needed
			// essentially walking back the matrix to get world space instead of local (camera) space
			mm::Mat44 trueMatrix = glm::inverse(static_cast<const glm::mat4&>(matrix));
#endif

			if(this_pointer->ScnPtr != nullptr) {
				if(this_pointer != this_pointer->ScnPtr->getCam(-1)) {
					// not our active cam, move on
					Orig(this_pointer, matrix);
					return;
					//camColor.a = 0.1f;
				} else {
					// the active camera!

					if(!freecam.isOn)
						freecam.matrix = trueMatrix; // put current cam matrix into the state

					if(freecam.isOn) {
						// use the matrix calculated in DoFreeCameraMovement
#if XENOMODS_CODENAME(bfsw)
						Orig(this_pointer, freecam.matrix);
#else
						mm::Mat44 inverse = glm::inverse(static_cast<const glm::mat4&>(freecam.matrix));
						Orig(this_pointer, inverse);
#endif
					} else {
						Orig(this_pointer, matrix);
					}
				}

				if(xenomods::DebugStuff::enableDebugRendering && freecam.isOn) {
					fw::debug::drawCompareZ(false);
					fw::debug::drawCamera(trueMatrix, camColor);
					fw::debug::drawCompareZ(true);
				}
			}
		}
	};

	struct UpdateFOVNearFar : skylaunch::hook::Trampoline<UpdateFOVNearFar> {
		static void Hook(ml::ScnObjCam* this_pointer) {
			if(this_pointer->ScnPtr != nullptr) {
				if(this_pointer == this_pointer->ScnPtr->getCam(-1)) {
					auto& freecamState = xenomods::CameraTools::Freecam;

					if(freecamState.isOn)
						this_pointer->AttrTransformPtr->fov = freecamState.fov; // put freecam info into the current camera
					else
						freecamState.fov = this_pointer->AttrTransformPtr->fov; // get fov from current camera
				}
			}

			Orig(this_pointer);
		}
	};

#if XENOMODS_CODENAME(bfsw)
	struct FrameworkCameraUpdate : skylaunch::hook::Trampoline<FrameworkCameraUpdate> {
		static void Hook(fw::Camera* this_pointer) {
			Orig(this_pointer);

			if(xenomods::CameraTools::Freecam.isOn) {
				xenomods::CameraTools::DoFreeCameraMovement(xenomods::CameraTools::Freecam.matrixUI);

				this_pointer->matrix = xenomods::CameraTools::Freecam.matrixUI;
				this_pointer->matrix2 = xenomods::CameraTools::Freecam.matrixUI;

				this_pointer->fov = xenomods::CameraTools::Freecam.fov;
			} else
				xenomods::CameraTools::Freecam.matrixUI = this_pointer->matrix;
		}
	};
#else
	struct FrameworkCameraUpdate : skylaunch::hook::Trampoline<FrameworkCameraUpdate> {
		// strictly handles fov updates
		static void Hook(fw::Camera* this_pointer) {
			Orig(this_pointer);

			if(xenomods::CameraTools::Freecam.isOn)
				this_pointer->fov = xenomods::CameraTools::Freecam.fov;
		}
	};

	struct BackupSetLookAt : skylaunch::hook::Trampoline<BackupSetLookAt> {
		static void Hook(fw::Camera* this_pointer, const mm::Vec3& pos, const mm::Vec3& target, float roll) {
			std::string camName = std::string(this_pointer->getName());
			//if(xenomods::DebugStuff::enableDebugRendering)
			//	xenomods::g_Logger->ToastInfo("freecamCamInfo" + camName, "{} prio {} fov {}", camName, this_pointer->CAMERA_PRIO, this_pointer->fov);

			if(xenomods::CameraTools::Freecam.isOn) {
				// 2 and Torna camera matricies are not world-space
				mm::Mat44 trueMatrix = glm::inverse(static_cast<const glm::mat4&>(xenomods::CameraTools::Freecam.matrix));
				this_pointer->matrix = trueMatrix;
				this_pointer->fov = xenomods::CameraTools::Freecam.fov;
			} else {
				Orig(this_pointer, pos, target, roll);

				// explicitly <= because multiple cameras can have the same prio
				if(camName != "SubViewCamera" && this_pointer->CAMERA_PRIO <= xenomods::CameraTools::highestCameraPrio) {
					xenomods::CameraTools::highestCameraPrio = this_pointer->CAMERA_PRIO;

					xenomods::CameraTools::Freecam.matrix = glm::inverse(static_cast<const glm::mat4&>(this_pointer->matrix));
					xenomods::CameraTools::Freecam.fov = this_pointer->fov;
				}
			}
		}
	};
#endif

} // namespace

namespace xenomods {

	CameraTools::FreecamState CameraTools::Freecam = {
		.isOn = false,
		.matrix = glm::identity<glm::mat4>(),
#if XENOMODS_CODENAME(bfsw)
		.matrixUI = glm::identity<glm::mat4>(),
#endif
		.fov = 45.f,
		.camSpeed = 1.f
	};

	unsigned int CameraTools::highestCameraPrio = -1;

	void CameraTools::DoFreeCameraMovement(glm::mat4& matrix) {
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
		fw::debug::drawFontFmtShadow(0, yPos += height, mm::Col4::white, "speed: {:.3f}", Freecam.camSpeed);
		fw::debug::drawFontFmtShadow(0, yPos += height, mm::Col4::white, "fov: {:.1f}", Freecam.fov);
#endif

		glm::mat4 newmat = glm::mat4(1.f);
		newmat = glm::translate(newmat, pos + move);
		newmat = glm::rotate(newmat, angle, axis);

		matrix = newmat;
	}

	void CameraTools::Initialize() {
		g_Logger->LogDebug("Setting up camera tools...");

#if XENOMODS_CODENAME(bfsw)
		SetCameraMatrix::HookAt(&ml::ScnObjCam::setWorldMatrix);
#else
		SetCameraMatrix::HookAt(&ml::ScnObjCam::setViewMatrix);
		BackupSetLookAt::HookAt(&fw::Camera::setLookAt);
#endif

		FrameworkCameraUpdate::HookAt(&fw::Camera::update);
		UpdateFOVNearFar::HookAt(&ml::ScnObjCam::updateFovNearFar);
	}

	void CameraTools::Update() {
		if(GetPlayer(2)->InputDownStrict(Keybind::FREECAM_TOGGLE)) {
			Freecam.isOn = !Freecam.isOn;
			g_Logger->ToastInfo(STRINGIFY(Freecam.isOn), "Freecam: {}", Freecam.isOn);
		}

		highestCameraPrio = -1;

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

#if XENOMODS_CODENAME(bfsw)
					// decompose existing matrix
					glm::decompose(static_cast<const glm::mat4&>(Freecam.matrixUI), scale, rot, pos, skew, perspective);
					newmat = glm::identity<glm::mat4>();
					newmat = glm::translate(newmat, pos);
					// just don't apply any rotation

					Freecam.matrixUI = newmat;
#endif
				}

				lastRollPress = now;
			}
		}

		if(Freecam.isOn)
			DoFreeCameraMovement(static_cast<glm::mat4&>(Freecam.matrix));
	}

	XENOMODS_REGISTER_MODULE(CameraTools);

} // namespace xenomods