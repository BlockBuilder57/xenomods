// Created by block on 1/1/24.

#pragma once

#include "../UpdatableModule.hpp"

#if XENOMODS_OLD_ENGINE
namespace xenomods {

	struct CompatabilityFix : public xenomods::UpdatableModule {
		void Initialize() override;
		bool NeedsUpdate() const override {
			return false;
		}
		void OnMapChange(unsigned short mapId) override;
	};

} // namespace xenomods
#endif