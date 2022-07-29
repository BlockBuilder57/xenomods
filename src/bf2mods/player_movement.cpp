#include "player_movement.hpp"

#include <bf2mods/gf/player_controller.hpp>
#include <bf2mods/prettyprinter.hpp>
#include <bf2mods/stuff/utils/debug_util.hpp>

#include "bf2logger.hpp"
#include "bf2mods/stuff/utils/util.hpp"
#include "bf2mods/utils.hpp"
#include "debug_stuff.hpp"
#include "state.hpp"
#include "skyline/logger/Logger.hpp"

namespace gf {

	namespace pc {

		void NormalizeMovementDeltas(gf::GfComPropertyPc* pcProperty) {
			if(bf2mods::GetState().moonJump) {
				static_cast<glm::vec3&>(pcProperty->velocityActual).y = 8.f;
				//dbgutil::logStackTrace();

				//---------- this function
				//7100a663e4 ai::Hfsm::update(void const*)+c8
				//7100656290 gf::GfComBehaviorPc::update(fw::UpdateInfo const&)+454
				//71006a1070 gf::GfObjUpdate::updateComponent(gf::GfObj*, fw::UpdateInfo const&)+c4
				//7100672384 gf::GfObj::update(fw::UpdateInfo const&)+20
				//71001c4b18 fw::UpdatableGroup::update(fw::UpdateInfo const&, unsigned int)+6c
				//71001c56cc fw::UpdatableObjectManager::update(fw::UpdateInfo const&, unsigned int)+74
				//71001b0964 fw::Framework::update()+1c8
			}

			if(bf2mods::GetState().options.movementSpeedMult == 1.0f)
				return;

			static_cast<glm::vec3&>(pcProperty->velocityDelta) *= bf2mods::GetState().options.movementSpeedMult;

			if(pcProperty->velocityDelta.XZLengthSqu() > 8.f) {
				glm::vec3 normalized = static_cast<glm::vec3>(pcProperty->velocityDelta.XZNormalized()) * 8.f * bf2mods::GetState().options.movementSpeedMult;
				normalized.y = static_cast<glm::vec3&>(pcProperty->velocityDelta).y;
				pcProperty->velocityDelta = normalized;
			}
		}

		GENERATE_SYM_HOOK(MoveUtilField_updateMoveJump, "_ZN2gf2pc13MoveUtilField14updateMoveJumpERKN2fw10UpdateInfoERNS_15GfComPropertyPcE", void, void* param_1, gf::GfComPropertyPc* pcProperty) {
			MoveUtilField_updateMoveJumpBak(param_1, pcProperty);
			gf::pc::NormalizeMovementDeltas(pcProperty);
		}
		GENERATE_SYM_HOOK(MoveUtilField_updateMoveRun, "_ZN2gf2pc13MoveUtilField13updateMoveRunERKN2fw10UpdateInfoERNS_15GfComPropertyPcE", void, void* param_1, gf::GfComPropertyPc* pcProperty) {
			MoveUtilField_updateMoveRunBak(param_1, pcProperty);
			gf::pc::NormalizeMovementDeltas(pcProperty);
		}
		GENERATE_SYM_HOOK(MoveUtilField_updateMoveSwim, "_ZN2gf2pc13MoveUtilField14updateMoveSwimERKN2fw10UpdateInfoERNS_15GfComPropertyPcE", void, void* param_1, gf::GfComPropertyPc* pcProperty) {
			MoveUtilField_updateMoveSwimBak(param_1, pcProperty);
			gf::pc::NormalizeMovementDeltas(pcProperty);
		}

		// FallDistance handles the animation+grunt the player makes when falling from a height
		GENERATE_SYM_HOOK(FallDistancePlugin_calcDistance, "_ZNK2gf2pc18FallDistancePlugin12calcDistanceERKN2mm4Vec3E", float, FallDamagePlugin* this_pointer, mm::Vec3* vec) {
			float height = FallDistancePlugin_calcDistanceBak(this_pointer, vec);
			//bf2mods::g_Logger->LogInfo("FallDistancePlugin::calcDistance called, would return %.2f", height);
			return bf2mods::GetState().options.disableFallDamage ? 0.f : height;
		}

		// Can't figure out how to properly set the actual max height, but this will do
		// Forcibly disables fall damage when any movement state tries to enable or disable it
		GENERATE_SYM_HOOK(StateUtil_setFallDamageDisable, "_ZN2gf2pc9StateUtil20setFallDamageDisableERNS_15GfComBehaviorPcEb", void, void* GfComBehaviorPc, bool param_2) {
			//bf2mods::g_Logger->LogInfo("StateUtil::setFallDamageDisable(GfComBehaviorPc: %p, bool: %s)", GfComBehaviorPc, bf2mods::format(param_2).c_str());
			StateUtil_setFallDamageDisableBak(GfComBehaviorPc, bf2mods::GetState().options.disableFallDamage ? true : param_2);
		}

	} // namespace pc

	GENERATE_SYM_HOOK(PlayerCameraTarget_writeTargetInfo, "_ZN2gf18PlayerCameraTarget15writeTargetInfoEv", void, gf::PlayerCameraTarget* this_pointer) {
		PlayerCameraTarget_writeTargetInfoBak(this_pointer);
		if(bf2mods::GetState().moonJump) {
			// makes the game always take the on ground path in gf::PlayerCamera::updateTracking
			this_pointer->inAir = false;
			// should stop the camera from suddenly jerking back to the maximum height moonjumped to
			this_pointer->surfaceHeight = static_cast<glm::vec3&>(this_pointer->moverPos).y;
			this_pointer->aboveWalkableGround = true;
		}
	}

} // namespace gf

namespace bf2mods::PlayerMovement {

	void Setup() {
		g_Logger->LogDebug("Setting up player movement hooks...");

		gf::pc::MoveUtilField_updateMoveJumpHook();
		gf::pc::MoveUtilField_updateMoveRunHook();
		gf::pc::MoveUtilField_updateMoveSwimHook();

		gf::pc::FallDistancePlugin_calcDistanceHook();
		gf::pc::StateUtil_setFallDamageDisableHook();

		gf::PlayerCameraTarget_writeTargetInfoHook();
	}

} // namespace bf2mods::PlayerMovement
