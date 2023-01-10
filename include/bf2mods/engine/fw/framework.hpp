//
// Created by block on 1/10/2023.
//

#pragma once

#include <bf2mods/utils.hpp>

namespace fw {

	struct Framework {
		INSERT_PADDING_BYTES(0x10);
		float mFrameRate;

		static void setUpdateSpeed(float speed);
	};

}