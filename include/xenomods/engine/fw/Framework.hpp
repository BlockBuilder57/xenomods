//
// Created by block on 1/10/2023.
//

#pragma once

#include <xenomods/Utils.hpp>

#include "UpdateInfo.hpp"

namespace fw {

	struct FrameworkConfig {
		int unk1;
		float mMaxUpdateFps;
		INSERT_PADDING_BYTES(0x34);
		INSERT_PADDING_BYTES(0x1); // is this one real?
	};

	struct Framework {
		INSERT_PADDING_BYTES(0x10);
		FrameworkConfig config;

		static void setUpdateSpeed(float speed);

		static UpdateInfo* getUpdateInfo();
	};

}