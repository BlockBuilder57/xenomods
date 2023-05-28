//
// Created by block on 5/27/23.
//

#include "BattleCheats.hpp"

#include "xenomods/DebugWrappers.hpp"
#include "xenomods/Logger.hpp"
#include "xenomods/NnFile.hpp"
#include "xenomods/Utils.hpp"

#include "xenomods/engine/btl/Damage.hpp"

#include <skylaunch/hookng/Hooks.hpp>

namespace {

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

}

namespace xenomods {

	void BattleCheats::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up battle cheats...");

		ModifyDamage::HookAt(&btl::BattleCharacter::NotifyDamage);
	}

#if XENOMODS_OLD_ENGINE
	XENOMODS_REGISTER_MODULE(BattleCheats);
#endif

} // namespace xenomods