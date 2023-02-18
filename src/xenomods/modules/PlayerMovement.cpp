#include "PlayerMovement.hpp"

#include <xenomods/DebugWrappers.hpp>
#include <xenomods/HidInput.hpp>
#include <xenomods/Logger.hpp>
#include <xenomods/Utils.hpp>
#include <skylaunch/hookng/Hooks.hpp>

#include "../State.hpp"
#include "../main.hpp"
#include "DebugStuff.hpp"
#include "xenomods/engine/fw/UpdateInfo.hpp"
#include "xenomods/engine/gf/PlayerController.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"
#include "xenomods/stuff/utils/util.hpp"

namespace {

	struct ApplyVelocityChanges : skylaunch::hook::Trampoline<ApplyVelocityChanges> {
		static void Hook(gf::GfComBehaviorPc* this_pointer, fw::UpdateInfo* updateInfo, gf::GfComPropertyPc* pcProperty) {
			//using enum gf::GfComPropertyPc::Flags;
			//bool flagInAir = (pcProperty->flags & static_cast<std::uint32_t>(InAir)) != 0;
			//bool flagOnWall = (pcProperty->flags & static_cast<std::uint32_t>(OnWall)) != 0;

			if(xenomods::PlayerMovement::moonJump)
				static_cast<glm::vec3&>(pcProperty->velocityActual).y = std::max(10.f, 10.f * (xenomods::PlayerMovement::movementSpeedMult / 64.f));

			auto& wish = static_cast<glm::vec3&>(pcProperty->velocityWish);

			wish *= xenomods::PlayerMovement::movementSpeedMult;

			//fw::debug::drawFontFmtShadow(0, 200, mm::Col4::white, "velAct {:2} (len {:.2f})", static_cast<const glm::vec3&>(pcProperty->velocityActual), pcProperty->velocityActual.XZLength());
			//fw::debug::drawFontFmtShadow(0, 216, mm::Col4::white, "velWsh {:2} (len {:.2f})", static_cast<const glm::vec3&>(pcProperty->velocityWish), pcProperty->velocityWish.XZLength());
			//fw::debug::drawFontFmtShadow(0, 232, mm::Col4::white, "who knows {:2} {:2}", static_cast<const glm::vec2&>(pcProperty->inputReal), static_cast<const glm::vec2&>(pcProperty->inputDupe));
			//fw::debug::drawFontFmtShadow(0, 248, mm::Col4::white, "flags: {:032b}", static_cast<std::uint32_t>(pcProperty->flags));
			//fw::debug::drawFontFmtShadow(0, 264, mm::Col4::white, "in air? {} on wall? {}", flagInAir, flagOnWall);

			Orig(this_pointer, updateInfo, pcProperty);

			wish /= xenomods::PlayerMovement::movementSpeedMult;
		}
	};

	struct DisableFallDamagePlugin : skylaunch::hook::Trampoline<DisableFallDamagePlugin> {
		static float Hook(gf::pc::FallDamagePlugin* this_pointer, mm::Vec3* vec) {
			return xenomods::PlayerMovement::disableFallDamage ? 0.f : Orig(this_pointer, vec);
		}
	};

	struct DisableStateUtilFallDamage : skylaunch::hook::Trampoline<DisableStateUtilFallDamage> {
		static void Hook(gf::GfComBehaviorPc* GfComBehaviorPc, bool param_2) {
			//xenomods::g_Logger->LogInfo("StateUtil::setFallDamageDisable(GfComBehaviorPc: {:p}, bool: {})", GfComBehaviorPc, param_2);
			Orig(GfComBehaviorPc, xenomods::PlayerMovement::disableFallDamage || param_2);
		}
	};

	struct CorrectCameraTarget : skylaunch::hook::Trampoline<CorrectCameraTarget> {
		static void Hook(gf::PlayerCameraTarget* this_pointer) {
			Orig(this_pointer);
			if(xenomods::PlayerMovement::moonJump) {
				// makes the game always take the on ground path in gf::PlayerCamera::updateTracking
				this_pointer->inAir = false;
				// should stop the camera from suddenly jerking back to the maximum height moonjumped to
				this_pointer->surfaceHeight = static_cast<glm::vec3&>(this_pointer->moverPos).y;
				this_pointer->aboveWalkableGround = true;
			}
		}
	};

}

namespace xenomods {

	bool PlayerMovement::moonJump = false;
	bool PlayerMovement::disableFallDamage = true;
	float PlayerMovement::movementSpeedMult = 1.f;

	void PlayerMovement::Initialize() {
		g_Logger->LogDebug("Setting up player movement hooks...");

		ApplyVelocityChanges::HookAt("_ZN2gf15GfComBehaviorPc19integrateMoveNormalERKN2fw10UpdateInfoERNS_15GfComPropertyPcE");

		DisableFallDamagePlugin::HookAt("_ZNK2gf2pc18FallDistancePlugin12calcDistanceERKN2mm4Vec3E");
		DisableStateUtilFallDamage::HookAt("_ZN2gf2pc9StateUtil20setFallDamageDisableERNS_15GfComBehaviorPcEb");

		CorrectCameraTarget::HookAt("_ZN2gf18PlayerCameraTarget15writeTargetInfoEv");
	}

	void PlayerMovement::Update() {
		moonJump = GetPlayer(1)->InputHeld(Keybind::MOONJUMP);

		if(GetPlayer(2)->InputDownStrict(Keybind::MOVEMENT_SPEED_UP)) {
			movementSpeedMult *= 2.0f;
			g_Logger->LogInfo("Movement speed multiplier set to {:.2f}", movementSpeedMult);
		} else if(GetPlayer(2)->InputDownStrict(Keybind::MOVEMENT_SPEED_DOWN)) {
			movementSpeedMult /= 2.0f;
			g_Logger->LogInfo("Movement speed multiplier set to {:.2f}", movementSpeedMult);
		} else if(GetPlayer(2)->InputDownStrict(Keybind::DISABLE_FALL_DAMAGE)) {
			disableFallDamage = !disableFallDamage;
			g_Logger->LogInfo("Disable fall damage: {}", disableFallDamage);
		}
	}

#if XENOMODS_CODENAME(bf2) || XENOMODS_CODENAME(ira)
	XENOMODS_REGISTER_MODULE(PlayerMovement);
#endif

} // namespace xenomods
