#include "BdatRandomizer.hpp"

#include "../State.hpp"
#include "../main.hpp"
#include "bf2mods/Logger.hpp"
#include "bf2mods/engine/bdat/Bdat.hpp"
#include "bf2mods/stuff/utils/util.hpp"
#include "nn/oe.h"
#include "version.h"

#include <skylaunch/hookng/Hooks.hpp>

namespace {
	static bool versionBufferPrepared = false;
	static char bdatVersionBuffer[16] {};

	struct MSTextHook : skylaunch::hook::Trampoline<MSTextHook> {
		static const char* Hook(unsigned char* pBdat, int n) {
			const char* sheetName = Bdat::getSheetName(pBdat);

			if(!strcmp(sheetName, "menu_ms")) {
				// I don't like hardcoding these but they don't seem to change between versions
				// it's unfortunately nicer (imo) than doing a string comparison
				if(n == 1830) {
					//nn::oe::DisplayVersion displayVersion;
					//nn::oe::GetDisplayVersion(&displayVersion);

					// TODO: probably use fmt
					if(!versionBufferPrepared) {
						std::snprintf(&bdatVersionBuffer[0], sizeof(bdatVersionBuffer) - 1, "(%s)", bf2mods::version::tagDirty);
						versionBufferPrepared = true;
					}

					return &bdatVersionBuffer[0];
				} else if(n == 1610) {
					// it says "Loading" in the japanese version too so I'm not allowed to moan about hardcoding this
					return "Loading (modded)";
				}
			}

			switch(bf2mods::BdatRandomizer::scrambleType) {
				using enum bf2mods::BdatRandomizer::BdatScrambleType;

				case ScrambleIndex:
					// scrambles the index of the ms text sheet
					return Orig(pBdat, (util::nnRand<int16_t>() % Bdat::getIdCount(pBdat)) + Bdat::getIdTop(pBdat));
				case ShowSheetName:
					return sheetName;
				case Off:
				default:
					return Orig(pBdat, n);
			}
		}
	};
} // namespace

namespace bf2mods {

	BdatRandomizer::BdatScrambleType BdatRandomizer::scrambleType = BdatRandomizer::BdatScrambleType::Off;

	void BdatRandomizer::Initialize() {
		g_Logger->LogDebug("Setting up BDAT randomizer...");

		// Hook stuff
		MSTextHook::HookAt(Bdat::getMSText);
	}

	void BdatRandomizer::Update() {
		if(btnDown(Keybind::BDAT_SCRAMBLETYPE_TOGGLE, p2Cur.Buttons, p2Prev.Buttons)) {
			underlying_value(scrambleType) += 1;

			if(scrambleType >= BdatScrambleType::Count)
				scrambleType = BdatScrambleType::Off;

			g_Logger->LogInfo("Bdat scramble type set to {}", scrambleType);
		}
	}

	BF2MODS_REGISTER_MODULE(BdatRandomizer);

} // namespace bf2mods
