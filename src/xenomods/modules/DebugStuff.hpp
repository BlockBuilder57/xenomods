#pragma once

#include "UpdatableModule.hpp"
#include "xenomods/engine/gf/MenuObject.hpp"

namespace xenomods {

	struct DebugStuff : public xenomods::UpdatableModule {
		static bool enableDebugRendering;
		static bool accessClosedLandmarks;

		static int bgmTrackIndex;

		static void DoMapJump(unsigned int mapjumpId);
		static void PlaySE(unsigned int soundEffect);
		static void ReturnTitle(unsigned int slot = -1);

		void Initialize() override;
		bool NeedsUpdate() const override {
			return true;
		}
		void Update(fw::UpdateInfo* updateInfo) override;
	};

} // namespace xenomods
