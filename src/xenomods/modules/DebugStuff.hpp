#pragma once

#include "UpdatableModule.hpp"
#include "xenomods/engine/gf/MenuObject.hpp"

namespace xenomods {

	struct DebugStuff : public xenomods::UpdatableModule {
		static bool enableDebugRendering;
		static bool enableDebugUnlockAll;
		static bool accessClosedLandmarks;

		static int tempInt;
		static int bgmTrackIndex;

		static unsigned short GetMapId();
		static std::string GetMapName(int id);
		static std::string GetMapName() { return GetMapName(GetMapId()); }

		static void DoMapJump(int mapjumpId);
		static void PlaySE(unsigned int soundEffect);
		static void ReturnTitle(unsigned int slot = -1);

		static void UpdateDebugRendering();

		void Initialize() override;
		bool NeedsUpdate() const override {
			return true;
		}
		void Update(fw::UpdateInfo* updateInfo) override;
	};

} // namespace xenomods
