#pragma once

#include "UpdatableModule.hpp"

namespace bf2mods {

	struct MenuViewer : public bf2mods::UpdatableModule {
		static bool enableUIRendering;

		void Initialize() override;
		bool NeedsUpdate() const override {
			return true;
		}
		void Update() override;
	};

} // namespace bf2mods
