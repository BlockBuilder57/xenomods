//
// Created by block on 1/18/2023.
//

#pragma once

#include "UpdatableModule.hpp"
#include <string>
#include <vector>

namespace bf2mods {

	struct EventDebugUtils : public bf2mods::UpdatableModule {

		static int RegistrationIndex;
		static int CurrentIndex;
		static unsigned long ActiveBits;
		static std::vector<std::string> FuncNames;

		static bool ShouldUpdate;

		void Initialize() override;
		bool NeedsUpdate() const override {
			return ShouldUpdate;
		}
		void Update() override;
	};

} // namespace bf2mods