// Created by block on 5/27/23.

#pragma once

#include "UpdatableModule.hpp"

namespace xenomods {

	struct BattleCheats : public xenomods::UpdatableModule {
		static bool disableVisionBGM;
		static bool disableTensionBGM;

		static void MenuSection();

		void Initialize() override;
		void OnConfigUpdate() override;
	};

} // namespace xenomods
