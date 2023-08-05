// Created by block on 5/27/23.

#pragma once

#include "UpdatableModule.hpp"

namespace xenomods {

	struct BattleCheats : public xenomods::UpdatableModule {
		static void MenuSection();

		void Initialize() override;
	};

} // namespace xenomods
