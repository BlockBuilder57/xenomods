// Created by block on 2/26/2023.

#pragma once

#include "UpdatableModule.hpp"

#if XENOMODS_CODENAME(bfsw)
namespace xenomods {

	struct DEDebugTests : public xenomods::UpdatableModule {
		void Initialize() override;
		bool NeedsUpdate() const override {
			return true;
		}
		void Update(fw::UpdateInfo* updateInfo) override;
	};

} // namespace xenomods
#endif
