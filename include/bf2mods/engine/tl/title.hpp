//
// Created by block on 7/21/2022.
//

#pragma once

#include "bf2mods/engine/gf/savegame.hpp"

namespace tl {

	class TitleMain {
	   public:
		static void returnTitle(gf::SAVESLOT slot);
		unsigned int getChapterIdFromSaveData();
		void* getSaveBuffer();
	};

} // namespace tl