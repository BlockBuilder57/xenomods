//
// Created by block on 5/27/23.
//

#include "BattleCheats.hpp"

#include "xenomods/ImGuiExtensions.hpp"
#include "xenomods/engine/btl/Character.hpp"
#include "xenomods/engine/fw/Document.hpp"
#include "xenomods/engine/game/Actor.hpp"
#include "xenomods/engine/game/BGM.hpp"
#include "xenomods/engine/game/Battle.hpp"
#include "xenomods/engine/gf/Party.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"

namespace {

#if XENOMODS_OLD_ENGINE

	static bool IsPlayerTakesBool = false;

	struct ModifyDamage : skylaunch::hook::Trampoline<ModifyDamage> {
		static void Hook(btl::BattleCharacter* this_pointer, btl::BattleCharacter::NOTIFY_DAMAGE_DATA& data, btl::BattleCharacter::NOTIFY_DAMAGE_RET& ret, const mm::Vec3& pos) {
			if(data.attacker != nullptr) {
				//xenomods::g_Logger->LogInfo("damage: {}, from us? {}", data.damage, data.attacker->IsBlade(false) || data.attacker->IsDriver(false));
				//xenomods::g_Logger->LogInfo("damage: {}, driver type: {}", data.damage, static_cast<int>(data.attacker->GetDriverType()));

				if(IsPlayerTakesBool && (data.attacker->IsBlade(false) || data.attacker->IsDriver(false)))
					data.damage *= xenomods::GetState().config.damagePlayerMult;
				else if(!IsPlayerTakesBool && (data.attacker->IsBlade() || data.attacker->IsDriver())) // older versions did not include a boolean argument
					data.damage *= xenomods::GetState().config.damagePlayerMult;
				else
					data.damage *= xenomods::GetState().config.damageEnemyMult;
			}

			return Orig(this_pointer, data, ret, pos);
		}
	};
#elif XENOMODS_CODENAME(bfsw)
	struct ModifyDamage : skylaunch::hook::Trampoline<ModifyDamage> {
		static void Hook(game::BattleDamageCalcurator* this_pointer, game::BattleDamageInfo* damageInfo) {
			//xenomods::g_Logger->LogInfo("damage: {}, x{}", damageInfo->damage, damageInfo->damageMult);

			bool fromPC = false;
			game::ActorAccessor attacker(*xenomods::DocumentPtr, damageInfo->handleAttacker);
			if(attacker.isValid()) {
				auto behavior = attacker.getBehaviorComponent();
				if(behavior->getRTTI()->isKindOf(&game::BehaviorPc::m_rtti))
					fromPC = true;
			}

			if(fromPC)
				damageInfo->damage *= xenomods::GetState().config.damagePlayerMult;
			else
				damageInfo->damage *= xenomods::GetState().config.damageEnemyMult;

			Orig(this_pointer, damageInfo);
		}
	};

	struct DisableVisionBGM : skylaunch::hook::Trampoline<DisableVisionBGM> {
		static std::uint16_t Hook(game::BGM* this_pointer, game::GameController& controller) {
			if (xenomods::BattleCheats::disableVisionBGM)
				controller.someFlags &= ~(1 << 3); // and with all but the vision bit

			return Orig(this_pointer, controller);
		}
	};

	struct DisableTensionBGM : skylaunch::hook::Trampoline<DisableTensionBGM> {
		static bool Hook(game::BGM* this_pointer, game::GameController& controller) {
			if (xenomods::BattleCheats::disableTensionBGM)
				return false;

			return Orig(this_pointer, controller);
		}
	};
#endif

} // namespace

namespace xenomods {

	bool BattleCheats::disableVisionBGM = false;
	bool BattleCheats::disableTensionBGM = false;

	void BattleCheats::MenuSection() {
		auto config = &GetState().config;
		ImGui::PushItemWidth(ImGui::GetFrameHeight() * 8.f);
		imguiext::InputFloatExt("Player damage multiplier", &config->damagePlayerMult, 1.f, 10.f, 2.f, "%.2f");
		imguiext::InputFloatExt("Enemy damage multiplier", &config->damageEnemyMult, 1.f, 10.f, 2.f, "%.2f");
		ImGui::PopItemWidth();

#if XENOMODS_CODENAME(bfsw)
		ImGui::Checkbox("Disable vision BGM change", &disableVisionBGM);
		ImGui::Checkbox("Disable low tension/death BGM change", &disableTensionBGM);
#endif

		static float partyGauge = 3.f;
		ImGui::PushItemWidth(ImGui::GetFrameHeight() * 5.f);
		ImGui::SliderFloat("Sections", &partyGauge, 0.f, 3.f);
		ImGui::SameLine();
		if (ImGui::Button("Set Party Gauge")) {
#if XENOMODS_OLD_ENGINE
			gf::GfGameParty::setPartyGauge(partyGauge * 100);
#elif XENOMODS_CODENAME(bfsw)
			if (xenomods::DocumentPtr != nullptr)
				game::ObjUtil::setPartyGauge(*xenomods::DocumentPtr, partyGauge * 100);
#endif
		}
		ImGui::PopItemWidth();
	}

	void BattleCheats::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up battle cheats...");
		OnConfigUpdate();

#if XENOMODS_OLD_ENGINE
		IsPlayerTakesBool = skylaunch::hook::detail::ResolveSymbolBase("_ZNK3btl15BattleCharacter8IsDriverEv") == skylaunch::hook::INVALID_FUNCTION_PTR;

		ModifyDamage::HookAt(&btl::BattleCharacter::NotifyDamage);
#elif XENOMODS_CODENAME(bfsw)
		ModifyDamage::HookAt(&game::BattleDamageCalcurator::calcCounterSpike); // last call in calcDamage

		DisableVisionBGM::HookAt("_ZNK4game3BGM14getBattleBGMIDERKNS_14GameControllerE");
		DisableTensionBGM::HookAt("_ZNK4game3BGM15testTensionZeroERKNS_14GameControllerE");
#endif

		auto modules = g_Menu->FindSection("modules");
		if(modules != nullptr) {
			auto section = modules->RegisterSection(STRINGIFY(BattleCheats), "Battle Cheats");
			section->RegisterRenderCallback(&MenuSection);
		}
	}

	void BattleCheats::OnConfigUpdate() {
		disableVisionBGM = GetState().config.disableBattleBGMChanges;
		disableTensionBGM = GetState().config.disableBattleBGMChanges;
	}

#if !XENOMODS_CODENAME(bf3)
	XENOMODS_REGISTER_MODULE(BattleCheats);
#endif

} // namespace xenomods