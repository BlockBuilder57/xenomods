// Created by block on 5/27/23.

#pragma once

#include "xenomods/Utils.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"

namespace btl {

	struct ARTS_DATA {};
	struct ARTS_SLOT_DATA {};

	class BattleCharacter {
	   public:
		struct NOTIFY_DAMAGE_DATA {
			BattleCharacter* attacker;
			ARTS_SLOT_DATA* artsSlotData;
			ARTS_DATA* artsData;
			int artHitNumber;
			uint totalHitNumber;
			float artHitWeightRate;
			float unk3;
			uint damage;
			int unk4;
			int unk5;
		};

		struct NOTIFY_DAMAGE_RET {};

		void NotifyDamage(NOTIFY_DAMAGE_DATA& data, NOTIFY_DAMAGE_RET& ret, mm::Vec3 const* pos);

		bool IsDriver(bool unk) const;
		bool IsBlade(bool unk) const;
		bool IsBoss(bool unk) const;
		bool IsNamed(bool unk) const;
		bool IsBossOrNamed(bool unk) const;
	};

}