// Created by block on 3/21/23.

#pragma once

#include "Object.hpp"

#if XENOMODS_OLD_ENGINE
namespace gf {

	struct RQ_SETUP_PARTY_DRIVER {
		std::uint16_t bdatId[10];
		std::uint32_t leader;
	};

	struct RQ_SETUP_PARTY_basegame {
		std::int32_t driver;
		std::int32_t activeBladeIdx;
		std::int16_t blades[6];
	};

	struct RQ_SETUP_PARTY_torna {
		std::int16_t lead;
		std::int16_t rear1;
		std::int16_t rear2;
	};

	struct RQ_SETUP_PARTY {
		RQ_SETUP_PARTY_basegame base[10];
		RQ_SETUP_PARTY_torna ira[10];
		std::uint32_t unk1;
		std::uint32_t basePartyLead;
		std::uint32_t unk3;
		std::uint32_t unk4;
	};

	class GfDataParty {
	   public:
	};

	class GfGameParty {
	   public:
		static void setPartyLeader(gf::GF_OBJ_HANDLE*);
		static void setPartyLeader(unsigned int, unsigned int);

		static void setPartyGauge(unsigned short); // 100 per section (300 max)

		static void reviveMembere();

		static void getCurrentPartyInfo(gf::RQ_SETUP_PARTY_DRIVER&);
		static void getCurrentPartyInfo(gf::RQ_SETUP_PARTY&);

		static gf::GF_OBJ_HANDLE getLeader();
		static gf::GF_OBJ_HANDLE getHandleMover(unsigned int);
		static GfComTransform* getLeaderTransform();
	};

} // namespace gf
#endif