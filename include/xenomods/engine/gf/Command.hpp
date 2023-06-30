// Created by block on 6/24/23.

#pragma once

#include "Party.hpp"

#if XENOMODS_OLD_ENGINE
namespace gf {

	class GfReqCommand {
	   public:
		static void reqChangeParty(const RQ_SETUP_PARTY& party);
		static void reqChangeParty(const RQ_SETUP_PARTY_DRIVER& party);
	};

}
#endif
