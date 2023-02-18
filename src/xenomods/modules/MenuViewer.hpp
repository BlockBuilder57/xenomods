#pragma once

#include "UpdatableModule.hpp"

namespace xenomods {

	struct MenuViewer : public xenomods::UpdatableModule {
		static bool enableUIRendering;
		static bool straightenFont;

		void Initialize() override;
		bool NeedsUpdate() const override {
			return true;
		}
		void Update() override;
	};

} // namespace xenomods
