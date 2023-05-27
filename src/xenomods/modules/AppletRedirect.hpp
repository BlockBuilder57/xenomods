#pragma once

#include "UpdatableModule.hpp"

namespace xenomods {
	struct AppletRedirect : public xenomods::UpdatableModule {
		void Initialize() override;
		static bool IsMeliaRecentGame();
	};
} // namespace xenomods
