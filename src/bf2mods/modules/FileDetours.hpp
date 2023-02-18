//
// Created by block on 1/9/2023.
//

#pragma once

#include "UpdatableModule.hpp"

namespace xenomods {

	struct FileDetours : public xenomods::UpdatableModule {
		void Initialize() override;
		bool NeedsUpdate() const override {
			return false;
		}
	};

} // namespace xenomods
