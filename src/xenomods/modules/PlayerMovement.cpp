#include "PlayerMovement.hpp"

#include <skylaunch/hookng/Hooks.hpp>
#include <xenomods/DebugWrappers.hpp>
#include <xenomods/HidInput.hpp>
#include <xenomods/Logger.hpp>
#include <xenomods/menu/Menu.hpp>
#include <xenomods/State.hpp>
#include <xenomods/Utils.hpp>

#include "DebugStuff.hpp"
#include "xenomods/engine/fw/Document.hpp"
#include "xenomods/engine/fw/UpdateInfo.hpp"
#include "xenomods/engine/game/CharacterController.hpp"
#include "xenomods/engine/game/ObjUtil.hpp"
#include "xenomods/engine/gf/Party.hpp"
#include "xenomods/engine/gf/PlayerController.hpp"
#include "xenomods/engine/mm/mtl/RTTI.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"
#include "xenomods/stuff/utils/util.hpp"

namespace {

#if XENOMODS_OLD_ENGINE
	struct ApplyVelocityChanges : skylaunch::hook::Trampoline<ApplyVelocityChanges> {
		static void Hook(gf::GfComBehaviorPc* this_pointer, fw::UpdateInfo* updateInfo, gf::GfComPropertyPc* pcProperty) {
			//using enum gf::GfComPropertyPc::Flags;
			//bool flagInAir = (pcProperty->flags & static_cast<std::uint32_t>(InAir)) != 0;
			//bool flagOnWall = (pcProperty->flags & static_cast<std::uint32_t>(OnWall)) != 0;

			if(xenomods::PlayerMovement::moonJump)
				static_cast<glm::vec3&>(pcProperty->velocityActual).y = std::max(10.f, 10.f * (xenomods::PlayerMovement::movementSpeedMult / 64.f));

			auto& wish = static_cast<glm::vec3&>(pcProperty->velocityWish);

			wish *= xenomods::PlayerMovement::movementSpeedMult;

			//xenomods::debug::drawFontFmtShadow(0, 200, mm::Col4::white, "velAct {:2} (len {:.2f})", static_cast<const glm::vec3&>(pcProperty->velocityActual), pcProperty->velocityActual.XZLength());
			//xenomods::debug::drawFontFmtShadow(0, 216, mm::Col4::white, "velWsh {:2} (len {:.2f})", static_cast<const glm::vec3&>(pcProperty->velocityWish), pcProperty->velocityWish.XZLength());
			//xenomods::debug::drawFontFmtShadow(0, 232, mm::Col4::white, "who knows {:2} {:2}", static_cast<const glm::vec2&>(pcProperty->inputReal), static_cast<const glm::vec2&>(pcProperty->inputDupe));
			//xenomods::debug::drawFontFmtShadow(0, 248, mm::Col4::white, "flags: {:032b}", static_cast<std::uint32_t>(pcProperty->flags));
			//xenomods::debug::drawFontFmtShadow(0, 264, mm::Col4::white, "in air? {} on wall? {}", flagInAir, flagOnWall);

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

#else

	bool ValidToChange(game::CharacterController* cc) {
		if(cc->actorAccessor != nullptr && cc->actorAccessor->isValid()) {
			auto behavior = cc->actorAccessor->getBehaviorComponent();
			if(behavior->getRTTI()->isKindOf(&game::BehaviorPc::m_rtti))
				return true;
		}
		return false;
	}

	struct ApplyVelocityChanges : skylaunch::hook::Trampoline<ApplyVelocityChanges> {
		static void Hook(game::CharacterController* this_pointer, mm::Vec3& pos, bool param_2) {
			if(ValidToChange(this_pointer)) {
				auto& impulse = static_cast<glm::vec3&>(pos);
				auto& velocity = static_cast<glm::vec3&>(this_pointer->velocity);

				impulse *= xenomods::PlayerMovement::movementSpeedMult;

				// undo the y changes when we're not on a wall
				if(!xenomods::bitMask(this_pointer->flags, game::CharacterController::Flags::pcClimb))
					impulse.y /= xenomods::PlayerMovement::movementSpeedMult;

				if(xenomods::PlayerMovement::moonJump) {
					velocity.y = 0;
					impulse.y = std::max(0.5f, 0.5f * (xenomods::PlayerMovement::movementSpeedMult / 64.f));
				}
			}

			Orig(this_pointer, pos, param_2);
		}
	};

	struct DisableFallDamage : skylaunch::hook::Trampoline<DisableFallDamage> {
		static float Hook(game::CharacterController* this_pointer) {
			return ValidToChange(this_pointer) && xenomods::PlayerMovement::disableFallDamage ? 0 : Orig(this_pointer);
		}
	};

#endif

} // namespace

namespace xenomods {

	bool PlayerMovement::moonJump = false;
	bool PlayerMovement::disableFallDamage = true;
	float PlayerMovement::movementSpeedMult = 1.f;
	PlayerMovement::WarpData PlayerMovement::Warp = {};

	glm::vec3 PlayerMovement::GetPartyPosition() {
#if XENOMODS_OLD_ENGINE
		gf::GfComTransform* trans = gf::GfGameParty::getLeaderTransform();
		if(trans != nullptr)
			return *trans->getPosition();
#else
		if(DocumentPtr == nullptr) {
			g_Logger->LogError("can't get party position cause no doc ptr!");
			return {};
		}

		unsigned int leadHandle = game::ObjUtil::getPartyHandle(*DocumentPtr, 0);
		if (leadHandle != 0) {
			game::CharacterController* control = game::ObjUtil::getCharacterController(*DocumentPtr, leadHandle);
			if (control != nullptr) {
				return control->position;
			}
		}
#endif
		return {};
	}
	void PlayerMovement::SetPartyPosition(glm::vec3 pos) {
#if XENOMODS_OLD_ENGINE
		gf::GfComTransform* trans = gf::GfGameParty::getLeaderTransform();
		if (trans != nullptr)
			trans->setPosition(pos);
#else
		if(DocumentPtr == nullptr) {
			g_Logger->LogError("can't set party position cause no doc ptr!");
			return;
		}

		unsigned int leadHandle = game::ObjUtil::getPartyHandle(*DocumentPtr, 0);
		if (leadHandle != 0) {
			game::CharacterController* control = game::ObjUtil::getCharacterController(*DocumentPtr, leadHandle);
			if (control != nullptr) {
				control->syncFrame();
				control->setWarp(pos, 5);
				control->clearLanding();
			}
		}
#endif
	}
	glm::quat PlayerMovement::GetPartyRotation() {
#if XENOMODS_OLD_ENGINE
		gf::GfComTransform* trans = gf::GfGameParty::getLeaderTransform();
		if(trans != nullptr)
			return *trans->getRotation();
#else
		if(DocumentPtr == nullptr) {
			g_Logger->LogError("can't get party position cause no doc ptr!");
			return {};
		}

		unsigned int leadHandle = game::ObjUtil::getPartyHandle(*DocumentPtr, 0);
		if (leadHandle != 0) {
			game::CharacterController* control = game::ObjUtil::getCharacterController(*DocumentPtr, leadHandle);
			if (control != nullptr) {
				return control->rotation;
			}
		}
#endif
		return {};
	}
	void PlayerMovement::SetPartyRotation(glm::quat rot) {
#if XENOMODS_OLD_ENGINE
		gf::GfComTransform* trans = gf::GfGameParty::getLeaderTransform();
		if (trans != nullptr)
			trans->setRotation(rot);
#else
		if(DocumentPtr == nullptr) {
			g_Logger->LogError("can't set party position cause no doc ptr!");
			return;
		}

		unsigned int leadHandle = game::ObjUtil::getPartyHandle(*DocumentPtr, 0);
		if (leadHandle != 0) {
			game::CharacterController* control = game::ObjUtil::getCharacterController(*DocumentPtr, leadHandle);
			if (control != nullptr) {
				control->setQuat(rot);
			}
		}
#endif
	}

	glm::vec3 PlayerMovement::GetPartyVelocity() {
#if XENOMODS_OLD_ENGINE
		// TODO
		/*gf::GF_OBJ_HANDLE handle = gf::GfGameParty::getLeader();
		if (handle.IsValid()) {
			g_Logger->LogInfo("Got handle - {}", reinterpret_cast<unsigned long>(handle.Ptr()));

			auto acc = gf::GfObjAcc(handle.Ptr());
			g_Logger->LogInfo("Supposed type: {}", (int)acc.getType());

			void* thing = gf::GfObjUtil::getProperty(handle.Ptr());
			g_Logger->LogInfo("What's the thing? {}", thing);
			auto* behavior = reinterpret_cast<gf::GfComPropertyPc*>(thing);
			if (behavior != nullptr) {
				g_Logger->LogInfo("where is its rtti? {}", reinterpret_cast<void*>(behavior->getRTTI()));
				if (behavior->getRTTI() != nullptr) {
					g_Logger->LogInfo("rtti says: {}", behavior->getRTTI()->szName);
					if (behavior->getRTTI()->isKindOf(&gf::GfComPropertyPc::m_rtti)) {
						g_Logger->LogInfo("Wario");
					}
				}
				else
					g_Logger->LogInfo("uh oh, no RTTI");
			}
		}*/
#else
		if(DocumentPtr == nullptr) {
			g_Logger->LogError("can't get party position cause no doc ptr!");
			return {};
		}

		unsigned int leadHandle = game::ObjUtil::getPartyHandle(*DocumentPtr, 0);
		if (leadHandle != 0) {
			game::CharacterController* control = game::ObjUtil::getCharacterController(*DocumentPtr, leadHandle);
			if (control != nullptr) {
				return control->velocity;
			}
		}
#endif
		return {};
	}
	void PlayerMovement::SetPartyVelocity(glm::vec3 vel) {
#if XENOMODS_OLD_ENGINE
		// TODO
#else
		if(DocumentPtr == nullptr) {
			g_Logger->LogError("can't set party position cause no doc ptr!");
			return;
		}

		unsigned int leadHandle = game::ObjUtil::getPartyHandle(*DocumentPtr, 0);
		if (leadHandle != 0) {
			game::CharacterController* control = game::ObjUtil::getCharacterController(*DocumentPtr, leadHandle);
			if (control != nullptr) {
				control->clearVelocity();
				control->addLinearVelocity(vel);
			}
		}
#endif
	}

	void PlayerMovement::SaveWarp() {
		Warp.position = GetPartyPosition();
		Warp.rotation = GetPartyRotation();
		Warp.velocity = GetPartyVelocity();
		Warp.initialized = true;
		g_Logger->ToastInfo("warp", "Saved warp at {:3}", Warp.position);
		g_Logger->ToastInfo("warp2", "(rot {} vel {})", Warp.rotation, Warp.velocity);
	}

	void PlayerMovement::LoadWarp() {
		if(Warp.initialized) {
			SetPartyPosition(Warp.position);
			if (Warp.rotation != glm::identity<glm::quat>())
				SetPartyRotation(Warp.rotation); // can cause fun errors
			SetPartyVelocity(Warp.velocity);
			g_Logger->ToastInfo("warp", "Warped party to {:3}", Warp.position);
			g_Logger->ToastInfo("warp2", "(rot {} vel {})", Warp.rotation, Warp.velocity);
		}
	}

	void OnMenuWarpUpdate() {
		PlayerMovement::Warp.initialized = true;
	}

	void PlayerMovement::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up player movement hooks...");

#if XENOMODS_OLD_ENGINE
		ApplyVelocityChanges::HookAt("_ZN2gf15GfComBehaviorPc19integrateMoveNormalERKN2fw10UpdateInfoERNS_15GfComPropertyPcE");

		DisableFallDamagePlugin::HookAt("_ZNK2gf2pc18FallDistancePlugin12calcDistanceERKN2mm4Vec3E");
		DisableStateUtilFallDamage::HookAt("_ZN2gf2pc9StateUtil20setFallDamageDisableERNS_15GfComBehaviorPcEb");

		CorrectCameraTarget::HookAt("_ZN2gf18PlayerCameraTarget15writeTargetInfoEv");
#elif XENOMODS_CODENAME(bfsw)
		ApplyVelocityChanges::HookAt(&game::CharacterController::applyMoveVec);
		DisableFallDamage::HookAt(&game::CharacterController::getFallHeight);
		// TODO: look into game::PcStateUtil::updateClimbMove for vertical climbing speed
#endif

#if !XENOMODS_CODENAME(bf3) // need to find these for 3
		auto modules = g_Menu->FindSection("modules");
		if (modules != nullptr) {
			auto section = modules->RegisterSection(STRINGIFY(PlayerMovement), "Player Movement");
			section->RegisterOption<bool>(disableFallDamage, "Disable fall damage");
			section->RegisterOption<float>(movementSpeedMult, "Movement speed multiplier");
			section->RegisterOption<void>("Save Warp", &PlayerMovement::SaveWarp);
			section->RegisterOption<void>("Load Warp", &PlayerMovement::LoadWarp);
			section->RegisterOption<float>(Warp.position.x, "Warp pos X", &OnMenuWarpUpdate);
			section->RegisterOption<float>(Warp.position.y, "Warp pos Y", &OnMenuWarpUpdate);
			section->RegisterOption<float>(Warp.position.z, "Warp pos Z", &OnMenuWarpUpdate);
		}
#endif
	}

	void PlayerMovement::Update(fw::UpdateInfo* updateInfo) {
		moonJump = GetPlayer(1)->InputHeld(Keybind::MOONJUMP);
	}

#if !XENOMODS_CODENAME(bf3)
	XENOMODS_REGISTER_MODULE(PlayerMovement);
#endif

} // namespace xenomods
