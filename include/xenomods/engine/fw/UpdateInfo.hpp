//
// Created by block on 1/18/2023.
//

#pragma once

#include <xenomods/Utils.hpp>

namespace fw {

	struct UpdateInfo {
		float unk;
#if XENOMODS_CODENAME(bfsw)
		float unk2;
#endif
		float deltaTime;
	};

}