// Created by block on 6/18/23.

#pragma once

#include <magic_enum.hpp>

#include <xenomods/Utils.hpp>

#include "xenomods/engine/fw/Document.hpp"
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
		unsigned long unk1;
		unsigned long count;
		DataPc* getMember(PcID pcid);
	};

}

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