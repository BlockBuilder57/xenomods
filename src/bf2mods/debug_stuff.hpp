#pragma once

#include "bf2mods/UpdatableModule.hpp"

#include <bf2mods/engine/gf/menu_object.hpp>

namespace bf2mods {

	struct DebugStuff : public bf2mods::UpdatableModule {
		static int bgmTrackIndex;

		static void DoMapJump(unsigned int mapjumpId);
		static void PlaySE(unsigned int soundEffect);
		static void ReturnTitle(unsigned int slot = -1);

		void Initialize() override;
		bool NeedsUpdate() const override { return true; }
		void Update() override;
	};

} // namespace bf2mods
