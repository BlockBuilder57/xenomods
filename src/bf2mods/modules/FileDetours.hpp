//
// Created by block on 1/9/2023.
//

#pragma once

#include "UpdatableModule.hpp"

namespace bf2mods {

	struct FileDetours : public bf2mods::UpdatableModule {
		void Initialize() override;
		bool NeedsUpdate() const override {
			return false;
		}
	};

} // namespace bf2mods
