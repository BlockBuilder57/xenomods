//
// Created by block on 1/18/2023.
//

#pragma once

#include <xenomods/Utils.hpp>

namespace fw {

	struct UpdateInfo {
#if XENOMODS_CODENAME(bfsw)
		float unk1;
#endif
#if XENOMODS_OLD_ENGINE
		float updateRate;
#endif
		float updateDelta;
		float updateRatio;
	};

}