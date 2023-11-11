#pragma once

#include "../UpdatableModule.hpp"

#if XENOMODS_CODENAME(bfsw)

namespace xenomods {

	struct AppletRedirect : public xenomods::UpdatableModule {
		void Initialize() override;
		static bool IsMeliaRecentGame();
	};

} // namespace xenomods

#endif
