// Created by block on 7/1/23.

#pragma once

#include <xenomods/Utils.hpp>

#if XENOMODS_OLD_ENGINE

namespace gf {

	enum class AOC_TYPE {
		Ira,
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

		Ver1_1 = 0x0101,

		Ira =              0x000000,
		NewQuests =        0x010000,
		NewRareBlade =     0x020000,
		NewChallengeMode = 0x030000,
		HelpfulItems =     0x040000,
	};

	class GfDataAoc {
	   public:
		static int getContentVersion(gf::AOC_TYPE type);
		static void debugSetContentVersion(gf::AOC_TYPE type, int ver);
	};

	class GfGameAoc {
	   public:
		bool isExistGameAoc(GAMEAOC aoc) const;
	};
}

#endif