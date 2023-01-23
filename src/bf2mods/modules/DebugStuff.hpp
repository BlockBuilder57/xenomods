#pragma once

#include "UpdatableModule.hpp"
#include "bf2mods/engine/gf/MenuObject.hpp"

namespace bf2mods {

	struct DebugStuff : public bf2mods::UpdatableModule {
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
		void Update() override;
	};

} // namespace bf2mods
