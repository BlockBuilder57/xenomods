//
// Created by block on 7/21/2022.
//

#pragma once

#include "xenomods/engine/gf/SaveGame.hpp"

namespace tl {

	class TitleMain {
	   public:
		unsigned int getChapterIdFromSaveData();
		void* getSaveBuffer();
		static void returnTitle(gf::SAVESLOT slot);
		void playTitleEvent(uint event_id);
	};

} // namespace tl