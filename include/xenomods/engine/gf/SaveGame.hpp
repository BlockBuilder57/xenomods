//
// Created by block on 7/21/2022.
//

#pragma once

namespace gf {

	enum SAVESLOT : unsigned {
		CurrentSlot = 0xFFFFFFFF,
		Slot0 = 0,
		Slot1 = 1,
		Slot2 = 2
	};

	class GfGameCond {
	   public:
		static bool isGameCond(unsigned int id);
		static bool isGameCondWithBlade(unsigned int id, unsigned int bladeId);
		static bool isGameCondWithIndex(unsigned int id, unsigned int idx, unsigned int idx2);
		static bool isGameCondWithMap(unsigned int id, unsigned int mapId);
	};

	class GfGameFlag {
	   public:
		static unsigned int getLocal(unsigned int bitSize, int id);
	};

	// size 1207696 (0x126D90)
	struct SDataGame {
	   public:
		unsigned int money;
		INSERT_PADDING_BYTES(0x126D90-4);
	};

	class GfDataStore {
	   public:
		static SDataGame* getDataGame();
	};

}
