//
// Created by block on 6/21/2022.
//

#include "camera_tools.hpp"

#include <bf2mods/apps/FrameworkLauncher.hpp>
#include <bf2mods/event/manager.hpp>
#include <bf2mods/fw/camera.hpp>
#include <bf2mods/ml/camera.hpp>
#include <bf2mods/mm/math_types.hpp>
#include <bf2mods/stuff/utils/debug_util.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/mat4x4.hpp>

#include "bf2logger.hpp"
#include "bf2mods/fw/debug.hpp"
#include "bf2mods/stuff/utils/util.hpp"
#include "plugin_main.hpp"
#include "skyline/logger/Logger.hpp"
#include "state.hpp"

namespace ml {

	template<auto backupFunction>
	void FreecamUpdateMatrix(ScnObjCam* this_pointer, mm::Mat44& matrix) {
		auto& state = bf2mods::GetState();
		mm::Col4 camColor = mm::Col4::White;

#if BF2MODS_CODENAME(bfsw)
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
				(*backupFunction)(this_pointer, matrix);
				camColor.a = 0.1f;
			} else {
				// the active camera!

				if(!state.freecam.isOn)
					state.freecam.matrix = trueMatrix; // put current cam matrix into the state

				if(state.freecam.isOn) {
					// use the matrix calculated in DoFreeCameraMovement
#if BF2MODS_CODENAME(bfsw)
					(*backupFunction)(this_pointer, freecamState.matrix);
#else
					mm::Mat44 inverse = glm::inverse(static_cast<const glm::mat4&>(state.freecam.matrix));
					(*backupFunction)(this_pointer, inverse);
#endif
				} else {
					(*backupFunction)(this_pointer, matrix);
				}
			}

			if(state.options.enableDebugRendering && state.freecam.isOn) {
				fw::debug::drawCompareZ(false);
				fw::debug::drawCamera(trueMatrix, camColor);
				fw::debug::drawCompareZ(true);
			}
		}
	}

	GENERATE_SYM_HOOK(ScnObjCam_setViewMatrix, "_ZN2ml9ScnObjCam13setViewMatrixERKN2mm5Mat44E", void, ScnObjCam* this_pointer, mm::Mat44& matrix) {
		FreecamUpdateMatrix<&ScnObjCam_setViewMatrixBak>(this_pointer, matrix);
	}

	GENERATE_SYM_HOOK(ScnObjCam_setWorldMatrix, "_ZN2ml9ScnObjCam14setWorldMatrixERKN2mm5Mat44E", void, ScnObjCam* this_pointer, mm::Mat44& matrix) {
		FreecamUpdateMatrix<&ScnObjCam_setWorldMatrixBak>(this_pointer, matrix);
	}

	GENERATE_SYM_HOOK(ScnObjCam_updateFovNearFar, "_ZN2ml9ScnObjCam16updateFovNearFarEv", void, ScnObjCam* this_pointer) {
		if(this_pointer->ScnPtr != nullptr) {
			if(this_pointer == this_pointer->ScnPtr->getCam(-1)) {
				auto& freecamState = bf2mods::GetState().freecam;

				if(freecamState.isOn)
					this_pointer->AttrTransformPtr->fov = freecamState.fov; // put freecam info into the current camera
				else
					freecamState.fov = this_pointer->AttrTransformPtr->fov; // get fov from current camera
			}
		}

		ScnObjCam_updateFovNearFarBak(this_pointer);
	}

} // namespace ml

#if !BF2MODS_CODENAME(bfsw)
namespace event {

	GENERATE_SYM_HOOK(Manager_update, "_ZN5event7Manager6updateEv", void, Manager* p_this) {
		if(!p_this->isPlayCancel() && bf2mods::GetState().options.enableDebugRendering)
			p_this->drawInfo();

		return Manager_updateBak(p_this);
	}

} // namespace event
#endif

namespace bf2mods::CameraTools {

	// for future reference:
	//auto seconds = nn::os::GetSystemTick()/19200000.;

	void DoFreeCameraMovement() {
		// controls:
		// Left stick: Y: forward/back, X: left/right
		// Right stick: XY: Look movement
		// LStick hold: Y: fov up/down
		// RStick hold: X: roll left/right

		// lazy usings
		using enum bf2mods::Keybind;
		using bf2mods::p1Cur;
		using bf2mods::p1Prev;
		using bf2mods::p2Cur;
		using bf2mods::p2Prev;

		auto freecamState = &bf2mods::GetState().freecam;

		glm::vec3 pos {};
		glm::quat rot {};
		glm::vec3 scale {};
		glm::vec3 skew {};
		glm::vec4 perspective {};

		// decompose existing matrix
		glm::decompose(static_cast<const glm::mat4&>(freecamState->matrix), scale, rot, pos, skew, perspective);

		glm::vec2 lStick = p2Cur.LAxis;
		glm::vec2 rStick = p2Cur.RAxis;

		// deadzone
		if(glm::length(lStick) < 0.15f)
			lStick = glm::zero<glm::vec2>();
		if(glm::length(rStick) < 0.15f)
			rStick = glm::zero<glm::vec2>();

		// movement
		glm::vec3 move {};
		if(btnHeld(FREECAM_FOVHOLD, p2Cur.Buttons)) {
			// holding down the button, so modify fov
			// note: game hard crashes during rendering when |fov| >= 180, it needs clamping
			freecamState->fov = std::clamp(freecamState->fov + -lStick.y * 0.25f, -179.f, 179.f);
		} else {
			move = { lStick.x, 0, -lStick.y };
			move = rot * move; // rotate movement to local space
		}

		if(btnDown(FREECAM_SPEED_UP, p2Cur.Buttons, p2Prev.Buttons))
			freecamState->camSpeed *= 2.f;
		else if(btnDown(FREECAM_SPEED_DOWN, p2Cur.Buttons, p2Prev.Buttons))
			freecamState->camSpeed /= 2.f;

		// multiply by cam speed
		move *= freecamState->camSpeed;

		//fw::debug::drawFont(500, 30, mm::Col4::White, "fov is: %.2f", freecamState->fov);

		// rotation
		glm::vec3 look {};
		float lookMult = 3.f;

		// slow the camera down at lower fovs
		if(std::abs(freecamState->fov) < 45.f)
			lookMult *= freecamState->fov / 45.f;

		if(btnHeld(FREECAM_ROLLHOLD, p2Cur.Buttons))
			look = { 0, 0, -rStick.x * 0.25f }; // only roll
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

		//bf2mods::g_Logger->LogInfo("euler: {}", glm::degrees(glm::eulerAngles(rot)));

		glm::mat4 newmat = glm::mat4(1.f);
		newmat = glm::translate(newmat, pos + move);
		newmat = glm::rotate(newmat, angle, axis);

		freecamState->matrix = newmat;
	}

	void Setup() {
		g_Logger->LogDebug("Setting up camera tools...");

#if BF2MODS_CODENAME(bfsw)
		ml::ScnObjCam_setWorldMatrixHook();
#else
		ml::ScnObjCam_setViewMatrixHook();
		event::Manager_updateHook();
#endif

		ml::ScnObjCam_updateFovNearFarHook();
	}

} // namespace bf2mods::CameraTools