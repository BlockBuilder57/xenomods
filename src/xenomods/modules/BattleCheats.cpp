//
// Created by block on 5/27/23.
//

#include "BattleCheats.hpp"

#include "xenomods/DebugWrappers.hpp"
#include "xenomods/Logger.hpp"
#include "xenomods/NnFile.hpp"
#include "xenomods/Utils.hpp"

#include "xenomods/engine/btl/Damage.hpp"
#include "xenomods/engine/fw/Document.hpp"
#include "xenomods/engine/game/Actor.hpp"
#include "xenomods/engine/game/Battle.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"

#include <skylaunch/hookng/Hooks.hpp>

namespace {

#if XENOMODS_OLD_ENGINE
	struct ModifyDamage : skylaunch::hook::Trampoline<ModifyDamage> {
		static void Hook(btl::BattleCharacter* this_pointer, btl::BattleCharacter::NOTIFY_DAMAGE_DATA& data, btl::BattleCharacter::NOTIFY_DAMAGE_RET& ret, const mm::Vec3& pos) {
			if (data.attacker != nullptr) {
				//xenomods::g_Logger->LogInfo("damage: {}, from us? {}", data.damage, data.attacker->IsBlade(false) || data.attacker->IsDriver(false));

				if (data.attacker->IsBlade(false) || data.attacker->IsDriver(false))
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
			if (attacker.isValid()) {
				auto behavior = attacker.getBehaviorComponent();
				if(behavior->getRTTI()->isKindOf(&game::BehaviorPc::m_rtti))
					fromPC = true;
			}

			if (fromPC)
				damageInfo->damage *= xenomods::GetState().config.damagePlayerMult;
			else
				damageInfo->damage *= xenomods::GetState().config.damageEnemyMult;

			Orig(this_pointer, damageInfo);
		}
	};
#endif

}

namespace xenomods {

	void BattleCheats::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up battle cheats...");

#if XENOMODS_OLD_ENGINE
		ModifyDamage::HookAt(&btl::BattleCharacter::NotifyDamage);
#elif XENOMODS_CODENAME(bfsw)
		ModifyDamage::HookAt(&game::BattleDamageCalcurator::calcCounterSpike); // last call in calcDamage
#endif
	}

	XENOMODS_REGISTER_MODULE(BattleCheats);

} // namespace xenomods