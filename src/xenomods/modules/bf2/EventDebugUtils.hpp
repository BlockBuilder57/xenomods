//
// Created by block on 1/18/2023.
//

#pragma once

#include <string>
#include <vector>

#include "../UpdatableModule.hpp"

namespace xenomods {

	struct EventDebugUtils : public xenomods::UpdatableModule {

		static int RegistrationIndex;
		static unsigned long ActiveBits;
		static std::vector<std::string> FuncNames;

		static bool ShouldUpdate;

		static void MenuSection();

		void Initialize() override;
		bool NeedsUpdate() const override {
			return ShouldUpdate;
		}
		void Update(fw::UpdateInfo* updateInfo) override;
	};

} // namespace xenomods