//
// Created by block on 1/10/2023.
//

#pragma once

#include <xenomods/Utils.hpp>

#include "Document.hpp"
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

		void update();
	};

	struct FrameworkController {
#if XENOMODS_CODENAME(bfsw)
		INSERT_PADDING_BYTES(0x90);
#elif XENOMODS_CODENAME(bf3)
		INSERT_PADDING_BYTES(0x50);
#endif
		bool isPaused;
	};

#if XENOMODS_NEW_ENGINE
	struct FrameworkUpdater {
		static void updateStd(const fw::Document& document, fw::FrameworkController* controller);

		static void update(const fw::Document& document, const fw::UpdateInfo& updateInfo, fw::FrameworkController* controller);
		static void updatePause(const fw::Document& document, const fw::UpdateInfo& updateInfo, fw::FrameworkController* controller);
	};
#endif

}