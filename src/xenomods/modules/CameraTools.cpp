//
// Created by block on 6/21/2022.
//

#include "CameraTools.hpp"
#include "DebugStuff.hpp"
#include "PlayerMovement.hpp"

#include "glm/gtx/matrix_decompose.hpp"
#include "glm/mat4x4.hpp"
#include "xenomods/ImGuiExtensions.hpp"
#include "xenomods/engine/apps/FrameworkLauncher.hpp"
#include "xenomods/engine/fw/Document.hpp"
#include "xenomods/engine/fw/Framework.hpp"
#include "xenomods/engine/game/MenuModelView.hpp"
#include "xenomods/engine/game/Utils.hpp"
#include "xenomods/engine/game/Scripts.hpp"
#include "xenomods/engine/gf/Party.hpp"
#include "xenomods/engine/ml/Scene.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"
#include "xenomods/stuff/utils/util.hpp"

namespace {

	struct PilotCameraLayers : skylaunch::hook::Trampoline<PilotCameraLayers> {
#if XENOMODS_NEW_ENGINE
		static void Hook(fw::CameraLayer* this_pointer, const fw::Document& document, const fw::UpdateInfo& updateInfo) {
#else
		static void Hook(fw::CameraLayer* this_pointer, const fw::UpdateInfo& updateInfo) {
#endif
			if(reinterpret_cast<void*>(this_pointer->listCamera.head) != &this_pointer->listCamera) {
				size_t ptrOffset = 0x10; //offsetof(fw::Camera, next);
				auto realHead = reinterpret_cast<fw::Camera*>(reinterpret_cast<u8*>(this_pointer->listCamera.head) - ptrOffset);
				//xenomods::g_Logger->LogDebug("list @ {} - head == {}, count {}", reinterpret_cast<void*>(&this_pointer->listCamera), reinterpret_cast<void*>(this_pointer->listCamera.head), this_pointer->listCamera.count);

				if(realHead == nullptr)
					return;

				while(true) {
					//xenomods::g_Logger->LogDebug("so no head? {} -> {}", reinterpret_cast<void*>(realHead), reinterpret_cast<void*>(realHead->next));
					//xenomods::g_Logger->LogDebug("say,  head? {} -> {}", reinterpret_cast<void*>(realHead), reinterpret_cast<void*>(realHead->prev));
					//dbgutil::logMemory(realHead, sizeof(fw::Camera));

					//if (realHead->getRTTI()->isKindOf(&fw::Camera::m_rtti)) {
					if(xenomods::CameraTools::Freecam.isOn) {
						realHead->matrix = glm::inverse(static_cast<const glm::mat4&>(xenomods::CameraTools::Freecam.matrix));
						realHead->fov = xenomods::CameraTools::Freecam.fov;

						//fw::debug::drawCompareZ(false);
						//fw::debug::drawCamera(glm::inverse(static_cast<const glm::mat4&>(realHead->matrix)), mm::Col4::cyan);
						//fw::debug::drawCompareZ(true);

						//std::string namey = std::string(realHead->getName());
						//xenomods::g_Logger->ToastInfo("ball" + namey, "{} prio {} fov {:.2f}", namey, realHead->CAMERA_PRIO, realHead->fov);

						//xenomods::debug::drawFontFmtShadow3D(xenomods::CameraTools::Meta.pos, mm::Col4::white, "Camera: {} prio {}", namey, realHead->CAMERA_PRIO);
					}
					//}

					if(realHead->prev == realHead->next)
						break;

					if(realHead->next == nullptr || reinterpret_cast<void*>(realHead->next) == &this_pointer->listCamera)
						break;

					realHead = reinterpret_cast<fw::Camera*>(reinterpret_cast<u8*>(realHead->next) - ptrOffset);
				}
			}

			if(xenomods::CameraTools::Freecam.isOn) {
				this_pointer->willLerp = true;
				this_pointer->lerpProgress = 999.f;
				this_pointer->matTarget = xenomods::CameraTools::Freecam.matrix;
				this_pointer->matCurrent = xenomods::CameraTools::Freecam.matrix;
			}

#if XENOMODS_NEW_ENGINE
			Orig(this_pointer, document, updateInfo);
#else
			Orig(this_pointer, updateInfo);
#endif

			if(xenomods::CameraTools::Freecam.isOn && this_pointer->objCam != nullptr) {
				this_pointer->objCam->AttrTransformPtr->fov = xenomods::CameraTools::Freecam.fov;
#if !XENOMODS_CODENAME(bf3)
				this_pointer->objCam->updateFovNearFar();
#endif
			}
		} // namespace
	};

	struct CopyCurrentCameraState : skylaunch::hook::Trampoline<CopyCurrentCameraState> {
		static void Hook(ml::ScnObjCam* this_pointer) {
			Orig(this_pointer);

#if !XENOMODS_CODENAME(bf3)
			if(this_pointer->ScnPtr != nullptr && this_pointer->AttrTransformPtr != nullptr && this_pointer == this_pointer->ScnPtr->getCam(-1)) {
#else
			if(this_pointer->AttrTransformPtr != nullptr) {
#endif
				if(!xenomods::CameraTools::Freecam.isOn) {
					// read state from current camera
					xenomods::CameraTools::Freecam.matrix = this_pointer->AttrTransformPtr->viewMatInverse;
					xenomods::CameraTools::Freecam.fov = this_pointer->AttrTransformPtr->fov;
					xenomods::CameraTools::UpdateMeta();
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
		.camSpeed = 8.f
	};

	CameraTools::CameraMeta CameraTools::Meta = {};

	void CameraTools::UpdateMeta() {
		glm::vec3 pos {};
		glm::quat rot {};
		glm::vec3 scale {};
		glm::vec3 skew {};
		glm::vec4 perspective {};

		// decompose existing matrix
		glm::decompose(static_cast<glm::mat4&>(Freecam.matrix), scale, rot, pos, skew, perspective);

		Meta.pos = pos;
		Meta.rot = rot;
		Meta.euler = glm::degrees(glm::eulerAngles(rot));

		glm::vec3 forward = { 0, 0, 1 };
		glm::vec3 up = { 0, 1, 0 };
		Meta.forward = rot * forward;
		Meta.up = rot * up;

		Meta.fov = Freecam.fov;
	}

	void DoFreeCameraMovement(float deltaTime) {
		// controls:
		// Left stick: Y: forward/back, X: left/right
		// Right stick: XY: Look movement

		// for future reference:
		//auto seconds = nn::os::GetSystemTick()/19200000.;

		// don't need to check if p1, we already did that
		HidInput* debugInput = HidInput::GetDebugInput();

		auto fc = &CameraTools::Freecam;
		auto meta = &CameraTools::Meta;

		glm::vec3 pos {};
		glm::quat rot {};
		glm::vec3 scale {};
		glm::vec3 skew {};
		glm::vec4 perspective {};

		// decompose existing matrix
		glm::decompose(static_cast<glm::mat4&>(fc->matrix), scale, rot, pos, skew, perspective);

		glm::vec2 lStick = debugInput->stateCur.LAxis;
		glm::vec2 rStick = debugInput->stateCur.RAxis;

		// deadzone
		if(glm::length(lStick) < 0.15f)
			lStick = glm::zero<glm::vec2>();
		if(glm::length(rStick) < 0.15f)
			rStick = glm::zero<glm::vec2>();

		// movement
		glm::vec3 move {};
		float fovMult = 30.f * deltaTime;

		// slow the zoom at lower fovs
		if(fc->fov != 0.0f && std::abs(fc->fov) < 20.f)
			fovMult *= std::lerp(0.01f, 1.0f, std::abs(fc->fov) / 20.f);

		if(debugInput->InputHeld(Keybind::CAMERA_COMBO)) {
			// holding down the button, so modify fov
			// note: game hard crashes during rendering when |fov| >= ~179.5 or == 0, it needs clamping
			fc->fov = std::clamp(fc->fov + -lStick.y * fovMult, -179.f, 179.f);
			if(fc->fov == 0)
				fc->fov = 0.001f;
		} else {
			move = { lStick.x, 0, -lStick.y };
			move = rot * move * deltaTime; // rotate movement to local space
			move *= fc->camSpeed;		   // multiply by cam speed
		}

		// rotation
		glm::vec3 look {};
		float lookMult = 70.f * deltaTime;
		float rollMult = 10.f * deltaTime;

		// slow the camera down at lower fovs
		if(fc->fov != 0.0f && std::abs(fc->fov) < 40.f)
			lookMult *= fc->fov / 40.f;

		if(debugInput->InputHeld(Keybind::CAMERA_COMBO))
			look = { 0, 0, -rStick.x * rollMult }; // only roll
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

		glm::mat4 newmat = glm::mat4(1.f);
		newmat = glm::translate(newmat, pos + move);
		newmat = glm::rotate(newmat, angle, axis);

		fc->matrix = newmat;
		CameraTools::UpdateMeta();
	}

	void TeleportPlayerToCamera() {
		if(xenomods::detail::IsModuleRegistered(STRINGIFY(PlayerMovement)))
			PlayerMovement::SetPartyPosition(CameraTools::Meta.pos);
	}

	void CameraTools::MenuSection() {
		ImGui::Checkbox("Freecam", &Freecam.isOn);

		ImGui::PushItemWidth(250.f);
		imguiext::InputFloatExt("Freecam speed", &Freecam.camSpeed, 1.f, 5.f, 2.f, "%.3f m/s");

		bool shouldUpdate = false;

		// icky short-circuit prevention...
		if(ImGui::DragFloat3("Camera pos", reinterpret_cast<float*>(&Meta.pos)))
			shouldUpdate = true;
		if(ImGui::DragFloat3("Camera rot", reinterpret_cast<float*>(&Meta.euler)))
			shouldUpdate = true;
		if(ImGui::DragFloat("Camera FOV", &Freecam.fov, 1, -179, 179))
			shouldUpdate = true;

		ImGui::PopItemWidth();

		if(shouldUpdate) {
			glm::quat newRot = glm::quat(glm::radians(CameraTools::Meta.euler));
			float angle = glm::angle(newRot);
			glm::vec3 axis = glm::axis(newRot);

			glm::mat4 newmat = glm::mat4(1.f);
			newmat = glm::translate(newmat, CameraTools::Meta.pos);
			newmat = glm::rotate(newmat, angle, axis);

			CameraTools::Freecam.matrix = newmat;

			// note: game hard crashes during rendering when |fov| >= ~179.5 or == 0, it needs clamping
			CameraTools::Freecam.fov = std::clamp(CameraTools::Freecam.fov, -179.f, 179.f);
			if(CameraTools::Freecam.fov == 0)
				CameraTools::Freecam.fov = 0.001f;

			CameraTools::Freecam.isOn = true;
		}

#if !XENOMODS_CODENAME(bf3)
		if(ImGui::Button("Teleport party lead to camera"))
			TeleportPlayerToCamera();
#endif
	}

	void CameraTools::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up camera tools...");

#if XENOMODS_OLD_ENGINE
		// intermittently reads the address as 0x0... let's just use the actual symbol for now
		// TODO: why *is* the function reference not exporting?
		PilotCameraLayers::HookAt("_ZN2fw11CameraLayer6updateERKNS_10UpdateInfoE");
#elif XENOMODS_CODENAME(bfsw)
		PilotCameraLayers::HookAt(&fw::CameraLayer::update);
#elif XENOMODS_CODENAME(bf3)
		// fw::CameraLayer::update
		if (version::RuntimeVersion() == version::SemVer::v2_0_0 || version::RuntimeVersion() == version::SemVer::v2_1_0)
			PilotCameraLayers::HookFromBase(0x7100013708);
		else if (version::RuntimeVersion() == version::SemVer::v2_1_1)
			PilotCameraLayers::HookFromBase(0x7100013718);
#endif

#if !XENOMODS_CODENAME(bf3)
		CopyCurrentCameraState::HookAt(&ml::ScnObjCam::updateFovNearFar);
#else
		// ml::ScnObjCam::updateFovNearFar
		if (version::RuntimeVersion() == version::SemVer::v2_0_0)
			CopyCurrentCameraState::HookFromBase(0x71012702ec);
		else if (version::RuntimeVersion() == version::SemVer::v2_1_0)
			CopyCurrentCameraState::HookFromBase(0x710127061c);
		else if (version::RuntimeVersion() == version::SemVer::v2_1_1)
			CopyCurrentCameraState::HookFromBase(0x710127065c);
#endif

		auto modules = g_Menu->FindSection("modules");
		if(modules != nullptr) {
			auto section = modules->RegisterSection(STRINGIFY(CameraTools), "Camera Tools");
			section->RegisterRenderCallback(&MenuSection);
			/*section->RegisterOption<bool>(Freecam.isOn, "Freecam");
			section->RegisterOption<float>(Freecam.camSpeed, "Freecam speed (m/s)");
			section->RegisterOption<float>(Meta.pos.x, "Camera pos X", &OnMenuMetaChange);
			section->RegisterOption<float>(Meta.pos.y, "Camera pos Y", &OnMenuMetaChange);
			section->RegisterOption<float>(Meta.pos.z, "Camera pos Z", &OnMenuMetaChange);
			section->RegisterOption<float>(Meta.euler.x, "Camera rot X", &OnMenuMetaChange);
			section->RegisterOption<float>(Meta.euler.y, "Camera rot Y", &OnMenuMetaChange);
			section->RegisterOption<float>(Meta.euler.z, "Camera rot Z", &OnMenuMetaChange);
			section->RegisterOption<float>(Meta.fov, "Camera FOV", &OnMenuMetaChange);
#if !XENOMODS_CODENAME(bf3)
			section->RegisterOption<void>("Teleport party lead to camera", &TeleportPlayerToCamera);
#endif*/
		}
	}

	void CameraTools::Update(fw::UpdateInfo* updateInfo) {
		HidInput* debugInput = HidInput::GetDebugInput();

		// if there's only one controller, let them freecam only when the menu is open
		if(debugInput == HidInput::GetPlayer(1) && !g_Menu->IsOpen())
			return;

		if(debugInput->InputDownStrict(Keybind::FREECAM_TOGGLE)) {
			Freecam.isOn = !Freecam.isOn;
			g_Logger->ToastInfo(STRINGIFY(Freecam.isOn), "Freecam: {}", Freecam.isOn);
		}

		if(Freecam.isOn) {
			bool speedChanged = false;
			if(debugInput->InputDownStrict(Keybind::FREECAM_SPEED_UP)) {
				Freecam.camSpeed *= 2.f;
				speedChanged = true;
			} else if(debugInput->InputDownStrict(Keybind::FREECAM_SPEED_DOWN)) {
				Freecam.camSpeed /= 2.f;
				speedChanged = true;
			}

			if(speedChanged)
				g_Logger->ToastInfo("freecamSpeed", "Freecam speed: {}m/s", Freecam.camSpeed);

			if(debugInput->InputDownStrict(Keybind::FREECAM_FOVRESET))
				Freecam.fov = 80;
			if(debugInput->InputDownStrict(Keybind::FREECAM_ROTRESET)) {
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

			DoFreeCameraMovement(updateInfo->updateDelta);

			if(debugInput->InputDownStrict(Keybind::FREECAM_TELEPORT))
				TeleportPlayerToCamera();
		}
	}

	XENOMODS_REGISTER_MODULE(CameraTools);

} // namespace xenomods