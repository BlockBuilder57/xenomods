// Created by block on 11/11/23.

#pragma once

#include <magic_enum/magic_enum.hpp>
#include <xenomods/Utils.hpp>

#include "xenomods/engine/fw/Document.hpp"
#include "xenomods/engine/ml/Time.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"
#include "xenomods/engine/mm/mtl/FixedVector.hpp"

#if XENOMODS_CODENAME(bfsw)
namespace game {

	enum class PcID : std::uint16_t {
		None,
		Shulk,
		Reyn,
		FioraHoms,
		Dunban,
		Sharla,
		Riki,
		Melia,
		FioraMachina,
		Dickson,
		Mumkhar,
		Alvis,
		DunbanPrelude,
		DunbanCopy,
		Kino,
		Nene,
		Ponspector_Wunwun,
		Ponspector_Tutu,
		Ponspector_Drydry,
		Ponspector_Fofora,
		Ponspector_Faifa,
		Ponspector_Hekasa,
		Ponspector_Setset,
		Ponspector_Teitei,
		Ponspector_Nonona,
		Ponspector_Dekadeka,
		Ponspector_Evelen,
		Ponspector_Tentoo,
	};

	enum class BdatId {
		// lol
	};

	class DataCharaStatus {
	   public:
		int level;
		int levelMax; // sorta read-only
		int hp;
		int unk1;
		float always100;
		unsigned int expLevel;
		unsigned int expTotal;
		unsigned int expBonus;
		unsigned int ap;
		unsigned int affinityCoins;
		float unk6;
		std::uint8_t tp_atk;
		std::uint8_t tp_arts;
		std::uint8_t tp_atkdmg;
		std::uint8_t tp_artsdmg;
		INSERT_PADDING_BYTES(0x10);
		unsigned int baseHp; // this being an int is intentional
		unsigned short baseStrength;
		unsigned short baseAgility;
		unsigned short baseEther;
		INSERT_PADDING_BYTES(0xA0-2);
		int hpMax;
		INSERT_PADDING_BYTES(0x4C);
	};

	class DataCharaValiableStatus {
	   public:
		static void setupMapEnemy(game::DataCharaStatus& status, const fw::Document& doc, BdatId pcid, unsigned short param_4, int param_5);
		static void setupPc(game::DataCharaStatus& status, const fw::Document& doc, PcID pcid, unsigned short param_4);
	};

	class DataFlag {
	   public:
		INSERT_PADDING_BYTES(0x1220);
	};

	struct DataQuest {
	   public:
		unsigned long unk1;
	};

	// size 289116 (0x42e28)
	class DataItem {
	   public:
		struct DataCommon {
			ushort unk1;
			ushort unk2;
			ushort itemId;
			ushort itemType;
			ushort stackCount;
			int addTime;
			bool isInInventory;
			bool isFavorite;
		};

		// Gems and mob crystals
		struct DataCrystal : public DataCommon {
			ushort slotSize;
			byte rankType;
			byte atrType;
			byte gemIds;
			INSERT_PADDING_BYTES(1);
			byte skillCount;
			INSERT_PADDING_BYTES(1);
			ushort unk3;
			byte arrSkills;
			byte skillRate;
			INSERT_PADDING_BYTES(4);
			byte alsoCountUnk;
			INSERT_PADDING_BYTES(3);
		};

		struct DataSlot {
			uint itemId;
			ushort skillId;
			bool isEquipped;
		};

		// Weapons and armor
		struct DataEquip : public DataCommon {
			INSERT_PADDING_BYTES(1);
			byte slotCount;
			INSERT_PADDING_BYTES(2);
			DataSlot slots[3];
		};

		DataEquip Weapons[500];
		DataEquip HeadArmor[500];
		DataEquip BodyArmor[500];
		DataEquip ArmArmor[500];
		DataEquip LeggArmor[500];
		DataEquip FootArmor[500];
		DataCrystal Crystals[550];
		DataCrystal Gems[550];
		DataCommon Collectables[500];
		DataCommon Materials[500];
		DataCommon KeyItems[500];
		DataCommon ArtBooks[500];
		INSERT_PADDING_BYTES(263279-228000);
		DataEquip TempWeapons[32];
		DataEquip TempHeadArmor[32];
		DataEquip TempBodyArmor[32];
		DataEquip TempArmArmor[32];
		DataEquip TempLeggArmor[32];
		DataEquip TempFootArmor[32];
		DataCrystal TempGems[35];
		INSERT_PADDING_BYTES(0x64);

		DataCommon* getItem(unsigned int);
	};

	// size 13440
	class DataPc {
	   public:
		PcID pcid;
		DataCharaStatus status;
		// a lot more
		INSERT_PADDING_BYTES(13124);
	};

	class DataParty {
	   public:
		PcID members[8];
		INSERT_PADDING_BYTES(0x8);
		unsigned long count;
		INSERT_PADDING_BYTES(0x8);
		INSERT_PADDING_BYTES(40);
		DataPc PCs[16];

		// much more...

		DataPc* getMember(PcID pcid);
	};

	class DataMenuSys {
		INSERT_PADDING_BYTES(0x654);
	};

	class DataBdat {};

	class DataParam {};

	class DataAI {};

	class DataSevFace {};

	class DataSpEff {};

	// Save data

	class DataSave {
	};

	// size 1390688 (0x153860)
	struct DataGameSave {
		INSERT_PADDING_BYTES(0x8);
		ml::TimeInfo timeInfo;
		INSERT_PADDING_BYTES(64);
		DataFlag dataFlag;
		DataQuest dataQuests[1300];
		DataItem dataItem;
		INSERT_PADDING_BYTES(0x10B1E4);
		unsigned int money; // 1383232 (0x151b40)
		INSERT_PADDING_BYTES(0x1D1C);
	};

	class DataGame {
	   public:
		DataGameSave dataGameSave;
		DataParty dataParty;
	};

} // namespace game

template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<game::PcID>(game::PcID value) noexcept {
	// clang-format off
	switch (value) {
		using enum game::PcID;

		//case Shulk:               return "Shulk"; break;
		//case Reyn:                return "Reyn"; break;
		case FioraHoms:           return "Fiora (Homs)"; break;
		//case Dunban:              return "Dunban"; break;
		//case Sharla:              return "Sharla"; break;
		//case Riki:                return "Riki"; break;
		//case Melia:               return "Melia"; break;
		case FioraMachina:        return "Fiora (Machina)"; break;
		//case Dickson:             return "Dickson"; break;
		//case Mumkhar:             return "Mumkhar"; break;
		//case Alvis:               return "Alvis"; break;
		case DunbanPrelude:       return "Dunban (Prelude)"; break;
		case DunbanCopy:          return "Dunban (Copy)"; break;
		//case Kino:                return "Kino"; break;
		//case Nene:                return "Nene"; break;
		case Ponspector_Wunwun:   return "Wunwun (Pon.)"; break;
		case Ponspector_Tutu:     return "Tutu (Pon.)"; break;
		case Ponspector_Drydry:   return "Drydry (Pon.)"; break;
		case Ponspector_Fofora:   return "Fofora (Pon.)"; break;
		case Ponspector_Faifa:    return "Faifa (Pon.)"; break;
		case Ponspector_Hekasa:   return "Hekasa (Pon.)"; break;
		case Ponspector_Setset:   return "Setset (Pon.)"; break;
		case Ponspector_Teitei:   return "Teitei (Pon.)"; break;
		case Ponspector_Nonona:   return "Nonona (Pon.)"; break;
		case Ponspector_Dekadeka: return "Dekadeka (Pon.)"; break;
		case Ponspector_Evelen:   return "Evelen (Pon.)"; break;
		case Ponspector_Tentoo:   return "Tentoo (Pon.)"; break;
	}
	// clang-format on
	return default_tag;
}

template<>
struct fmt::formatter<game::PcID> : fmt::formatter<std::string_view> {
	template<typename FormatContext>
	inline auto format(game::PcID pcid, FormatContext& ctx) {
		using enum game::PcID;

		// just checks for invalids
		if (pcid < None || pcid > Ponspector_Tentoo)
			return "Unknown - " + std::to_string(xenomods::underlying_value(pcid));

		return magic_enum::enum_name(pcid);
	}
};
#endif