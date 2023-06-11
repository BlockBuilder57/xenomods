// Created by block on 5/27/23.

#pragma once

#include "xenomods/Utils.hpp"
#include "xenomods/engine/gf/Object.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"

namespace btl {

	struct ARTS_DATA {};
	struct ARTS_SLOT_DATA {};
	struct GROUP_DATA {};

	// they added stuff to the vtable between 1.0.0 and 2.0.0
	// i hate supporting games that update
	/*class CharacterParameter {
	   public:
		virtual void Reset();
		virtual void Initialize(gf::GF_OBJ_HANDLE* handle);
		virtual void InitializeArtsPalette(int num);
		virtual void CheckChangeArtsPalette();
		virtual void BattleStart(int param_1);
		virtual void BattleEnd();
		virtual void BattleEndInitialize();
		virtual void PreUpdate(const fw::UpdateInfo& updateInfo);
		virtual void Update(const fw::UpdateInfo& updateInfo, bool param_2);
		virtual void PostUpdate(const fw::UpdateInfo& updateInfo);
		virtual int GetAttribute() const;
		virtual int GetBladeIndex() const;
		virtual int GetWeaponID(int num) const;
		virtual int GetEquipAccessory() const;
		virtual int GetIdeaLevel() const;
		virtual void* GetArtsPaletteDataConst(int num) const;
		virtual int GetArtsPaletteNum() const;
		virtual void ResetRecast();
		virtual void WritebackRecast(btl::GROUP_DATA*);
		virtual int GetWeakAttributeFlag() const;
		virtual bool InitializeEnhance() const;
		virtual bool IsPlayerParameter() const;
		virtual bool IsEnemyParameter() const;
	};*/

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
		bool IsDriver() const;
		bool IsBlade() const; // old version
		bool IsBoss(bool unk) const;
		bool IsNamed(bool unk) const;
		bool IsBossOrNamed(bool unk) const;
		bool IsControlCharacter() const;

		enum class DriverType {
			Type0,
			Type1,
			Type2,
			Type3,
			NotADriver,
		};

		DriverType GetDriverType() const;
	};

}