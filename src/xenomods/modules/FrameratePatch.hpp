// Created by block on 3/19/23.

#pragma once

#include "UpdatableModule.hpp"

namespace xenomods {

	struct FrameratePatch : public xenomods::UpdatableModule {
		void Initialize() override;
		bool NeedsUpdate() const override {
			return false;
		}
	};

} // namespace xenomods