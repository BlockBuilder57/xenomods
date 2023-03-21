// Created by block on 3/21/23.

#pragma once

#include "Object.hpp"

namespace gf {

	struct RQ_SETUP_PARTY_DRIVER {
		std::uint16_t bdatId[10];
		std::uint32_t leader;
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

		static GfComTransform* getLeaderTransform();
	};

}