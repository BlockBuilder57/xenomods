//
// Created by block on 1/10/2023.
//

#pragma once

#include <xenomods/Utils.hpp>

namespace fw {

	struct Framework {
		INSERT_PADDING_BYTES(0x10);
		float mFrameRate;

		static void setUpdateSpeed(float speed);
	};

}