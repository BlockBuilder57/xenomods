#include "player_movement.hpp"

#include <bf2mods/engine/gf/player_controller.hpp>

#include "bf2mods/bf2logger.hpp"
#include "bf2mods/debug_wrappers.hpp"
#include "bf2mods/stuff/utils/debug_util.hpp"
#include "bf2mods/stuff/utils/util.hpp"
#include "bf2mods/utils.hpp"
#include "debug_stuff.hpp"
#include "plugin_main.hpp"
#include "skylaunch/logger/Logger.hpp"
#include "state.hpp"

namespace gf {

	GENERATE_SYM_HOOK(GfComPropertyPc_writeBack, "_ZN2gf15GfComPropertyPc9writeBackEv", void, gf::GfComPropertyPc* this_pointer) {
		GfComPropertyPc_writeBackBak(this_pointer);

		// set the velocity for moonjump
		if(bf2mods::PlayerMovement::moonJump)
			static_cast<glm::vec3&>(this_pointer->velocityActual).y = 10.f;

		// wish never carries a Y component
		auto& wish = static_cast<glm::vec3&>(this_pointer->velocityWish);
		auto& input = static_cast<glm::vec2&>(this_pointer->inputReal);

		// if we're not inputting, just don't do anything
		if (glm::length(input) > 0) {
			wish *= bf2mods::PlayerMovement::movementSpeedMult;
			float speedLimit = 8.f * std::max(1.f, bf2mods::PlayerMovement::movementSpeedMult);

			if(glm::length(wish) >= speedLimit)
				wish = glm::normalize(wish) * speedLimit;
		}

		//fw::debug::drawFontFmtShadow(0, 216, mm::Col4::White, "velWsh PST {:2} (len {:.2f})", static_cast<const glm::vec3&>(this_pointer->velocityWish), this_pointer->velocityWish.XZLength());
		//fw::debug::drawFontFmtShadow(0, 232, mm::Col4::White, "who knows {:2} {:2}", static_cast<const glm::vec2&>(this_pointer->inputReal), static_cast<const glm::vec2&>(this_pointer->inputDupe));
	}

	namespace pc {

		// FallDistance handles the animation+grunt the player makes when falling from a height
		GENERATE_SYM_HOOK(FallDistancePlugin_calcDistance, "_ZNK2gf2pc18FallDistancePlugin12calcDistanceERKN2mm4Vec3E", float, FallDamagePlugin* this_pointer, mm::Vec3* vec) {
			float height = FallDistancePlugin_calcDistanceBak(this_pointer, vec);
			//bf2mods::g_Logger->LogInfo("FallDistancePlugin::calcDistance called, would return {:.2f}", height);
			return bf2mods::PlayerMovement::disableFallDamage ? 0.f : height;
		}

		// Can't figure out how to properly set the actual max height, but this will do
		// Forcibly disables fall damage when any movement state tries to enable or disable it
		GENERATE_SYM_HOOK(StateUtil_setFallDamageDisable, "_ZN2gf2pc9StateUtil20setFallDamageDisableERNS_15GfComBehaviorPcEb", void, void* GfComBehaviorPc, bool param_2) {
			//bf2mods::g_Logger->LogInfo("StateUtil::setFallDamageDisable(GfComBehaviorPc: {:p}, bool: {})", GfComBehaviorPc, param_2);
			StateUtil_setFallDamageDisableBak(GfComBehaviorPc, bf2mods::PlayerMovement::disableFallDamage ? true : param_2);
		}

	} // namespace pc

	GENERATE_SYM_HOOK(PlayerCameraTarget_writeTargetInfo, "_ZN2gf18PlayerCameraTarget15writeTargetInfoEv", void, gf::PlayerCameraTarget* this_pointer) {
		PlayerCameraTarget_writeTargetInfoBak(this_pointer);
		if(bf2mods::PlayerMovement::moonJump) {
			// makes the game always take the on ground path in gf::PlayerCamera::updateTracking
			this_pointer->inAir = false;
			// should stop the camera from suddenly jerking back to the maximum height moonjumped to
			this_pointer->surfaceHeight = static_cast<glm::vec3&>(this_pointer->moverPos).y;
			this_pointer->aboveWalkableGround = true;
		}
	}

} // namespace gf

namespace bf2mods {

	bool PlayerMovement::moonJump = false;
	bool PlayerMovement::disableFallDamage = true;
	float PlayerMovement::movementSpeedMult = 1.f;

	void PlayerMovement::Initialize() {
		g_Logger->LogDebug("Setting up player movement hooks...");

		gf::GfComPropertyPc_writeBackHook();

		gf::pc::FallDistancePlugin_calcDistanceHook();
		gf::pc::StateUtil_setFallDamageDisableHook();

		gf::PlayerCameraTarget_writeTargetInfoHook();
	}

	void PlayerMovement::Update() {
		moonJump = btnHeld(Keybind::MOONJUMP, p1Cur.Buttons);

		if(btnDown(Keybind::MOVEMENT_SPEED_UP, p2Cur.Buttons, p2Prev.Buttons)) {
			movementSpeedMult *= 2.0f;
			g_Logger->LogInfo("Movement speed multiplier set to {:.2f}", movementSpeedMult);
		} else if(btnDown(Keybind::MOVEMENT_SPEED_DOWN, p2Cur.Buttons, p2Prev.Buttons)) {
			movementSpeedMult /= 2.0f;
			g_Logger->LogInfo("Movement speed multiplier set to {:.2f}", movementSpeedMult);
		} else if(btnDown(Keybind::DISABLE_FALL_DAMAGE, p2Cur.Buttons, p2Prev.Buttons)) {
			disableFallDamage = !disableFallDamage;
			g_Logger->LogInfo("Disable fall damage: {}", disableFallDamage);
		}
	}

#if BF2MODS_CODENAME(bf2) || BF2MODS_CODENAME(ira)
	BF2MODS_REGISTER_MODULE(PlayerMovement);
#endif

} // namespace bf2mods
