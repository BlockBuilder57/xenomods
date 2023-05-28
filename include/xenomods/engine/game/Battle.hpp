// Created by block on 5/28/23.

#pragma once

#if XENOMODS_CODENAME(bfsw)
namespace game {

	struct DataAttack {};

	struct BattleDamageInfo {
		DataAttack* something;
		INSERT_PADDING_BYTES(0x8);
		unsigned int handleAttacker;
		unsigned int handleTarget;
		float damageOrig;
		float damageMult;
		int damage;
		INSERT_PADDING_BYTES(0x8);
		unsigned int flags;
	};

	class BattleDamageCalcurator {
	   public:
		void calcDamage();

		void calcSupportImpl(BattleDamageInfo& damageInfo);
		void addAttackEffect(BattleDamageInfo& damageInfo);
		void calcAttackEffect(BattleDamageInfo& damageInfo);
		void calcCounterSpike(BattleDamageInfo& damageInfo);

		float calcBaseDamage(int param_1, int param_2);
		float calcDamageRate();
		float calcCriticalDamageRate();

		float getMachineDamageRate();
	};

}
#endif