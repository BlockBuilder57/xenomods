#pragma once

#include "bf2mods/UpdatableModule.hpp"

namespace bf2mods {

	struct BdatRandomizer : public bf2mods::UpdatableModule {
		void Initialize() override;
		bool NeedsUpdate() const override { return true; }
		void Update() override;
	};

} // namespace bf2mods
