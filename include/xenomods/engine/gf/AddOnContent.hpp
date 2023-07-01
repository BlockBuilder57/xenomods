// Created by block on 7/1/23.

#pragma once

#include <xenomods/Utils.hpp>

#if XENOMODS_OLD_ENGINE

namespace gf {

	enum class AOC_TYPE {
		Torna,
		NewQuests,
		NewRareBlade,
		NewChallengeMode,
		HelpfulItems
	};

	enum class GAMEAOC {
		Patch1 = 0x01,
		Patch2 = 0x02,
		Patch3 = 0x03,
		Patch4 = 0x04,

		Major1 = 0x0100,
		Major2 = 0x0200,
		Major3 = 0x0300,
		Major4 = 0x0400,

		Ira = 0x30000,
	};

	class GfDataAoc {
	   public:
		static int getContentVersion(gf::AOC_TYPE type);
		static void debugSetContentVersion(gf::AOC_TYPE type, int ver);
	};
}

#endif