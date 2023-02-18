//
// Created by block on 7/21/2022.
//

#pragma once

#include "xenomods/engine/gf/SaveGame.hpp"

namespace tl {

	class TitleMain {
	   public:
		static void returnTitle(gf::SAVESLOT slot);
		unsigned int getChapterIdFromSaveData();
		void* getSaveBuffer();
	};

} // namespace tl