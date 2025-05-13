//
// Created by block on 6/21/2022.
//

#include "CameraTools.hpp"

#include "DebugStuff.hpp"
#include "PlayerMovement.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/matrix_decompose.hpp"
#undef GLM_ENABLE_EXPERIMENTAL
#include "glm/mat4x4.hpp"
#include "xenomods/ImGuiExtensions.hpp"
#include "xenomods/engine/apps/FrameworkLauncher.hpp"
#include "xenomods/engine/fw/Document.hpp"
#include "xenomods/engine/fw/Framework.hpp"
#include "xenomods/engine/game/MenuModelView.hpp"
#include "xenomods/engine/game/Scripts.hpp"
#include "xenomods/engine/game/Utils.hpp"
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
			if(xenomods::CameraTools::Settings.freecamOn) {
				realHead->matrix = glm::inverse(static_cast<const glm::mat4&>(xenomods::CameraTools::CamState.matrix));
				realHead->fov = xenomods::CameraTools::CamState.fov;

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

	if(xenomods::CameraTools::Settings.freecamOn) {
		this_pointer->willLerp = true;
		this_pointer->lerpProgress = 999.f;
		this_pointer->matTarget = xenomods::CameraTools::CamState.matrix;
		this_pointer->matCurrent = xenomods::CameraTools::CamState.matrix;
	}

#if XENOMODS_NEW_ENGINE
	Orig(this_pointer, document, updateInfo);
#else
			Orig(this_pointer, updateInfo);
#endif

	if(xenomods::CameraTools::Settings.freecamOn && this_pointer->objCam != nullptr) {
		this_pointer->objCam->AttrTransformPtr->fov = xenomods::CameraTools::CamState.fov;
#if !XENOMODS_CODENAME(bf3)
		this_pointer->objCam->updateFovNearFar();
#endif
	}
} // namespace
}
;

struct CopyCurrentCameraState : skylaunch::hook::Trampoline<CopyCurrentCameraState> {
	static void Hook(ml::ScnObjCam* this_pointer) {
		Orig(this_pointer);

#if !XENOMODS_CODENAME(bf3)
		if(this_pointer->ScnPtr != nullptr && this_pointer->AttrTransformPtr != nullptr && this_pointer == this_pointer->ScnPtr->getCam(-1)) {
#else
			if(this_pointer->AttrTransformPtr != nullptr) {
#endif
			if(!xenomods::CameraTools::Settings.freecamOn) {
				// read state from current camera
				xenomods::CameraTools::CamState.matrix = this_pointer->AttrTransformPtr->viewMatInverse;
				xenomods::CameraTools::CamState.fov = this_pointer->AttrTransformPtr->fov;
				xenomods::CameraTools::UpdateMeta();
			}
		}
	}
};
}
; // namespace

namespace xenomods {

	CameraTools::FreecamSettings CameraTools::Settings = {
		.freecamOn = false,
		.relativeToPlayer = false,
		.moveAxis = FreecamSettings::MoveAxis::XZ,
		.comboMoveAxis = FreecamSettings::MoveAxis::XY,
		.isFreezePos = { false, false, false },
		.isGlobalPos = { false, false, false },
		.isGlobalRot = { false, true, false },
		.camSpeed = 8.f,
		.enableTargeting = false,
		.targetPos = {}
	};

	CameraTools::CameraState CameraTools::CamState = {
		.matrix = glm::identity<glm::mat4>(),
		.fov = 40.f
	};

	CameraTools::CameraMeta CameraTools::CamMeta = {};

	glm::vec3 lastPlayerPos = {};
	glm::vec3 relativePlayerDelta = {};

	void CameraTools::UpdateMeta() {
		glm::vec3 pos {};
		glm::quat rot {};
		glm::vec3 scale {};
		glm::vec3 skew {};
		glm::vec4 perspective {};

		// decompose existing matrix
		glm::decompose(static_cast<glm::mat4&>(CamState.matrix), scale, rot, pos, skew, perspective);

		CamMeta.pos = pos;
		CamMeta.rot = rot;
		CamMeta.euler = glm::degrees(glm::eulerAngles(rot));

		glm::vec3 forward = { 0, 0, 1 };
		glm::vec3 up = { 0, 1, 0 };
		CamMeta.forward = rot * forward;
		CamMeta.up = rot * up;

		CamMeta.fov = CamState.fov;
	}

	void DoFreeCameraMovement(float deltaTime) {
		// for future reference:
		//auto seconds = nn::os::GetSystemTick()/19200000.;

		// don't need to check if p1, we already did that
		HidInput* debugInput = HidInput::GetDebugInput();

		auto set = &CameraTools::Settings;
		auto cs = &CameraTools::CamState;
		auto cm = &CameraTools::CamMeta;

		glm::vec3 pos {};
		glm::quat rot {};
		glm::vec3 scale {};
		glm::vec3 skew {};
		glm::vec4 perspective {};

		// decompose existing matrix
		glm::decompose(static_cast<glm::mat4&>(cs->matrix), scale, rot, pos, skew, perspective);

		glm::vec2 lStick = debugInput->stateCur.LAxis;
		glm::vec2 rStick = debugInput->stateCur.RAxis;

		// deadzone
		const float STICK_DEADZONE = 0.15f;
		if(glm::length(lStick) < STICK_DEADZONE)
			lStick = glm::zero<glm::vec2>();
		if(glm::length(rStick) < STICK_DEADZONE)
			rStick = glm::zero<glm::vec2>();

		// fov changing
		float fovMult = 30.f * deltaTime;

		// double the deadzone for the right stick X here to prevent accidental FOV changing when rolling
		bool shouldRoll = glm::abs(rStick.x) >= (STICK_DEADZONE * 2.f);
		bool shouldChangeFOV = !shouldRoll;

		// slow the zoom at lower fovs
		if(cs->fov != 0.0f && std::abs(cs->fov) < 20.f)
			fovMult *= std::lerp(0.01f, 1.0f, std::abs(cs->fov) / 20.f);

		if(debugInput->InputHeld(Keybind::CAMERA_COMBO) && shouldChangeFOV) {
			// holding down the combo, so modify fov
			// note: game hard crashes during rendering when |fov| >= ~179.5 or == 0, it needs clamping
			cs->fov = std::clamp(cs->fov + -rStick.y * fovMult, -179.f, 179.f);
			if(cs->fov == 0)
				cs->fov = 0.001f;
		}

		// movement
		glm::vec3 move { 0, 0, 0 };
		glm::vec3 perAxisMove { 0, 0, 0 };

		CameraTools::FreecamSettings::MoveAxis moveAxis = debugInput->InputHeld(Keybind::CAMERA_COMBO) ? set->comboMoveAxis : set->moveAxis;

		switch(moveAxis) {
			case CameraTools::FreecamSettings::MoveAxis::XZ:
				move = { lStick.x, 0, -lStick.y };
				break;
			case CameraTools::FreecamSettings::MoveAxis::XY:
				move = { lStick.x, lStick.y, 0 };
				break;
			case CameraTools::FreecamSettings::MoveAxis::YZ:
				move = { 0, lStick.y, -lStick.x };
				break;
		}

		perAxisMove += (set->isGlobalPos[0] ? glm::identity<glm::quat>() : rot) * glm::vec3(move.x, 0, 0);
		perAxisMove += (set->isGlobalPos[1] ? glm::identity<glm::quat>() : rot) * glm::vec3(0, move.y, 0);
		perAxisMove += (set->isGlobalPos[2] ? glm::identity<glm::quat>() : rot) * glm::vec3(0, 0, move.z);

		if (set->isFreezePos[0])
			perAxisMove.x = 0;
		if (set->isFreezePos[1])
			perAxisMove.y = 0;
		if (set->isFreezePos[2])
			perAxisMove.z = 0;

		move = perAxisMove * deltaTime; // rotate movement to local space
		move *= set->camSpeed;          // multiply by cam speed
		if (set->relativeToPlayer)      // add player movement
			move = move + relativePlayerDelta;

		// rotation
		if (set->enableTargeting) {
			// if targeting, just look at our target
			if (set->targetPos != pos + move)
				rot = glm::inverse(glm::lookAt(pos + move, set->targetPos, {0, 1, 0}));

			// i hope this fixes the crashes
			if (isnan(rot)[0])
				rot = glm::identity<glm::quat>();
		}
		else {
			glm::vec3 look {};
			float lookMult = 70.f * deltaTime;
			float rollMult = 20.f * deltaTime;

			// slow the camera down at lower fovs
			if(cs->fov != 0.0f && std::abs(cs->fov) < 40.f)
				lookMult *= cs->fov / 40.f;

			if(debugInput->InputHeld(Keybind::CAMERA_COMBO)) {
				if (shouldRoll)
					look = { 0, 0, -rStick.x * rollMult }; // only roll
			}
			else
				look = { rStick.y * lookMult, -rStick.x * lookMult, 0 }; // pitch and yaw

			// pitch is default in local space
			// yaw is default in world space
			// roll is default in local space
			float pitchDeg = glm::radians(look.x);
			glm::quat pitchRot = glm::angleAxis(pitchDeg, (set->isGlobalRot[0] ? glm::identity<glm::quat>() : rot) * glm::vec3(1, 0, 0));
			float yawDeg = glm::radians(look.y);
			glm::quat yawRot = glm::angleAxis(yawDeg, (set->isGlobalRot[1] ? glm::identity<glm::quat>() : rot) * glm::vec3(0, 1, 0));
			float rollDeg = glm::radians(look.z);
			glm::quat rollRot = glm::angleAxis(rollDeg, (set->isGlobalRot[2] ? glm::identity<glm::quat>() : rot) * glm::vec3(0, 0, 1));

			// apply rotations
			rot = yawRot * pitchRot * rollRot * rot;
		}

		// get angle+axis to rotate the matrix by
		float angle = glm::angle(rot);
		glm::vec3 axis = glm::axis(rot);

		glm::mat4 newmat = glm::mat4(1.f);
		newmat = glm::translate(newmat, pos + move);
		newmat = glm::rotate(newmat, angle, axis);

		cs->matrix = newmat;
		CameraTools::UpdateMeta();
	}

	// Buttons and menu functionality
	void ResetState(bool resetPos = false, bool resetRot = false, bool resetFOV = false) {
		if (resetPos || resetRot) {
			glm::vec3 pos {};
			glm::quat rot {};
			glm::vec3 scale {};
			glm::vec3 skew {};
			glm::vec4 perspective {};

			// decompose existing matrix
			glm::decompose(static_cast<const glm::mat4&>(xenomods::CameraTools::CamState.matrix), scale, rot, pos, skew, perspective);

			glm::mat4 newmat = glm::identity<glm::mat4>();
			if (!resetPos)
				newmat = glm::translate(newmat, pos);

			if (!resetRot) {
				// get angle+axis to rotate the matrix by
				float angle = glm::angle(rot);
				glm::vec3 axis = glm::axis(rot);

				newmat = glm::rotate(newmat, angle, axis);
			}

			xenomods::CameraTools::CamState.matrix = newmat;
		}

		if (resetFOV) {
			xenomods::CameraTools::CamState.fov = 80.f;
		}

		xenomods::CameraTools::UpdateMeta();
	}

	void TeleportPlayerToCamera() {
		if(xenomods::detail::IsModuleRegistered(STRINGIFY(PlayerMovement)))
			PlayerMovement::SetPartyPosition(CameraTools::CamMeta.pos);
	}

	void CameraTools::MenuSettings() {
		ImGui::SeparatorText("Movement");

#if !XENOMODS_CODENAME(bf3)
		ImGui::Checkbox("Relative to Player", &Settings.relativeToPlayer);
#endif

		ImGui::PushItemWidth(64.f);
		imguiext::EnumComboBox("Move type", &Settings.moveAxis);
		ImGui::SameLine();
		imguiext::EnumComboBox("L+R Move type", &Settings.comboMoveAxis);
		ImGui::PopItemWidth();

		ImGui::Checkbox("Freeze X", &Settings.isFreezePos[0]);
		ImGui::SameLine();
		ImGui::Checkbox("Freeze Y", &Settings.isFreezePos[1]);
		ImGui::SameLine();
		ImGui::Checkbox("Freeze Z", &Settings.isFreezePos[2]);

		ImGui::Checkbox("Global X", &Settings.isGlobalPos[0]);
		ImGui::SameLine();
		ImGui::Checkbox("Global Y", &Settings.isGlobalPos[1]);
		ImGui::SameLine();
		ImGui::Checkbox("Global Z", &Settings.isGlobalPos[2]);

		imguiext::InputFloatExt("Freecam speed", &Settings.camSpeed, 1.f, 5.f, 2.f, "%.3f m/s");

		ImGui::SeparatorText("Rotation");

		//ImGui::Checkbox("Global Pitch", &Settings.isGlobalRot[0]);
		//ImGui::SameLine();
		ImGui::Checkbox("Global Roll", &Settings.isGlobalRot[1]);
		//ImGui::SameLine();
		//ImGui::Checkbox("Global Yaw", &Settings.isGlobalRot[2]);

		ImGui::SeparatorText("Targeting");

		ImGui::Checkbox("Enable Targeting", &Settings.enableTargeting);
#if !XENOMODS_CODENAME(bf3)
		ImGui::Checkbox("Follow Player Position", &Settings.targetFollowPlayer);
#endif

		if (ImGui::Button("Set target from camera position")) {
			Settings.targetPos = CamMeta.pos;
		}

		ImGui::DragFloat3("Target Pos", reinterpret_cast<float*>(&Settings.targetPos));

		ImGui::Separator();
	}

	void CameraTools::MenuSection() {
		ImGui::Checkbox("Freecam", &Settings.freecamOn);

		ImGui::PushItemWidth(250.f);

		if (Settings.freecamOn && ImGui::CollapsingHeader("Control Options")) {
			MenuSettings();
		}

		// icky short-circuit prevention...
		bool shouldUpdate = false;

		if(ImGui::DragFloat3("Pos", reinterpret_cast<float*>(&CamMeta.pos)))
			shouldUpdate = true;
		ImGui::SameLine();
		if (ImGui::Button("Reset Pos")) {
			ResetState(true, false, false);
		}

		if(ImGui::DragFloat3("Rot", reinterpret_cast<float*>(&CamMeta.euler)))
			shouldUpdate = true;
		ImGui::SameLine();
		if (ImGui::Button("Reset Rot")) {
			ResetState(false, true, false);
		}

		if(ImGui::DragFloat("FOV", &CamState.fov, 1, -179, 179))
			shouldUpdate = true;
		ImGui::SameLine();
		if (ImGui::Button("Reset FOV")) {
			ResetState(false, false, true);
		}

		ImGui::PopItemWidth();

		if(shouldUpdate) {
			glm::quat newRot = glm::quat(glm::radians(CameraTools::CamMeta.euler));
			float angle = glm::angle(newRot);
			glm::vec3 axis = glm::axis(newRot);

			glm::mat4 newmat = glm::mat4(1.f);
			newmat = glm::translate(newmat, CameraTools::CamMeta.pos);
			newmat = glm::rotate(newmat, angle, axis);

			CameraTools::CamState.matrix = newmat;

			// note: game hard crashes during rendering when |fov| >= ~179.5 or == 0, it needs clamping
			CameraTools::CamState.fov = std::clamp(CameraTools::CamState.fov, -179.f, 179.f);
			if(CameraTools::CamState.fov == 0)
				CameraTools::CamState.fov = 0.001f;

			CameraTools::Settings.freecamOn = true;
		}

#if !XENOMODS_CODENAME(bf3)
		ImGui::Separator();
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
		if(version::RuntimeVersion() == version::SemVer::v2_0_0 || version::RuntimeVersion() == version::SemVer::v2_1_0 || version::RuntimeVersion() == version::SemVer::v2_2_0)
			PilotCameraLayers::HookFromBase(0x7100013708);
		else if(version::RuntimeVersion() == version::SemVer::v2_1_1)
			PilotCameraLayers::HookFromBase(0x7100013718);
#endif

#if !XENOMODS_CODENAME(bf3)
		CopyCurrentCameraState::HookAt(&ml::ScnObjCam::updateFovNearFar);
#else
		// ml::ScnObjCam::updateFovNearFar
		if(version::RuntimeVersion() == version::SemVer::v2_0_0)
			CopyCurrentCameraState::HookFromBase(0x71012702ec);
		else if(version::RuntimeVersion() == version::SemVer::v2_1_0)
			CopyCurrentCameraState::HookFromBase(0x710127061c);
		else if(version::RuntimeVersion() == version::SemVer::v2_1_1)
			CopyCurrentCameraState::HookFromBase(0x710127065c);
		else if(version::RuntimeVersion() == version::SemVer::v2_2_0)
			CopyCurrentCameraState::HookFromBase(0x71012711cc);
#endif

		auto modules = g_Menu->FindSection("modules");
		if(modules != nullptr) {
			auto section = modules->RegisterSection(STRINGIFY(CameraTools), "Camera Tools");
			section->RegisterRenderCallback(&MenuSection);
		}
	}

	void CameraTools::Update(fw::UpdateInfo* updateInfo) {
		HidInput* debugInput = HidInput::GetDebugInput();

		// if there's only one controller, let them freecam only when the menu is open
		if(debugInput == HidInput::GetPlayer(1) && !g_Menu->IsOpen())
			return;

		if(debugInput->InputDownStrict(Keybind::FREECAM_TOGGLE)) {
			Settings.freecamOn = !Settings.freecamOn;
			g_Logger->ToastInfo(STRINGIFY(Freecam.freecamOn), "Freecam: {}", Settings.freecamOn);
		}

		if(Settings.freecamOn) {
			bool speedChanged = false;
			if(debugInput->InputDownStrict(Keybind::FREECAM_SPEED_UP)) {
				Settings.camSpeed *= 2.f;
				speedChanged = true;
			} else if(debugInput->InputDownStrict(Keybind::FREECAM_SPEED_DOWN)) {
				Settings.camSpeed /= 2.f;
				speedChanged = true;
			}

			if(speedChanged)
				g_Logger->ToastInfo("freecamSpeed", "Freecam speed: {}m/s", Settings.camSpeed);

			if(debugInput->InputDownStrict(Keybind::FREECAM_FOVRESET))
				ResetState(false, false, true);
			if(debugInput->InputDownStrict(Keybind::FREECAM_ROTRESET)) {
				ResetState(false, true, false);
			}

#if !XENOMODS_CODENAME(bf3)
			if (xenomods::detail::IsModuleRegistered(STRINGIFY(PlayerMovement))) {
				glm::vec3* pos = PlayerMovement::GetPartyPosition();

				if (Settings.targetFollowPlayer && pos != nullptr)
					Settings.targetPos = *pos + glm::vec3(0, 1, 0);

				if (pos != nullptr) {
					relativePlayerDelta = *pos - lastPlayerPos;
				}
				else {
					// reset these, the party doesn't exist anymore
					lastPlayerPos = relativePlayerDelta = glm::zero<glm::vec3>();
				}
			}
#endif

			DoFreeCameraMovement(updateInfo->updateDelta);

#if !XENOMODS_CODENAME(bf3)
			if (xenomods::detail::IsModuleRegistered(STRINGIFY(PlayerMovement))) {
				glm::vec3* pos = PlayerMovement::GetPartyPosition();
				if (pos != nullptr)
					lastPlayerPos = *pos;
			}
#endif

			if(debugInput->InputDownStrict(Keybind::FREECAM_TELEPORT))
				TeleportPlayerToCamera();
		}
	}

	XENOMODS_REGISTER_MODULE(CameraTools);

} // namespace xenomods