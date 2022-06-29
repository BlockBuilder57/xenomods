#include "bdat_randomizer.hpp"

#include <version.h>
#include "bf2logger.hpp"

#include "bf2mods/stuff/utils/util.hpp"
#include "nn/oe.h"
#include "plugin.hpp"
#include "skyline/logger/Logger.hpp"

// Bdat syms & hooks
namespace Bdat {

	// These functions aren't hooked by us;
	// instead we *resolve* them when starting up
	// so we can just use them as is, just as if we were game code.

	std::uint64_t (*getIdCount)(std::uint8_t*);

	std::uint64_t (*getIdTop)(std::uint8_t*);

	char* (*getSheetName)(std::uint8_t*);

	GENERATE_SYM_HOOK(getMSText, "_ZN4Bdat9getMSTextEPhi", const char*, std::uint8_t* bdatData, int n) {
		//skyline::logger::s_Instance->LogFormat("Bdat::getMSText(bdat: %p, n: %d)", bdatData, n);
		const char* sheetName = Bdat::getSheetName(bdatData);

		if(!strcmp(sheetName, "menu_ms")) {
			// I don't like hardcoding these but they don't seem to change between versions
			// it's unfortunately nicer (imo) than doing a string comparison
			if(n == 1830) {
				//nn::oe::DisplayVersion displayVersion;
				//nn::oe::GetDisplayVersion(&displayVersion);

				std::stringstream ss;
				ss << "(" << bf2mods::version::tagDirty << ") ";

				return ss.str().c_str();
			} else if(n == 1610) {
				// it says "Loading" in the japanese version too so I'm not allowed to moan about hardcoding this
				return "Loading (modded)";
			}
		}

		using enum bf2mods::Options::BdatScrambleType;

		switch(bf2mods::Plugin::getSharedStatePtr()->options.bdatScrambleType) {
			case ScrambleIndex:
				// scrambles the index of the ms text sheet
				return Bdat::getMSTextBak(bdatData, (util::nnRand<int16_t>() % Bdat::getIdCount(bdatData)) + Bdat::getIdTop(bdatData));
			case ShowSheetName:
				return sheetName;
			case Off:
			default:
				return Bdat::getMSTextBak(bdatData, n);
		}
	}

	// todo more stuff to randomize more things
} // namespace Bdat

namespace bf2mods {

	void SetupBdatRandomizer() {
		g_Logger->LogInfo("Setting up BDAT randomizer...");

		util::ResolveSymbol<decltype(Bdat::getIdCount)>(&Bdat::getIdCount, "_ZN4Bdat10getIdCountEPh");
		util::ResolveSymbol<decltype(Bdat::getIdTop)>(&Bdat::getIdTop, "_ZN4Bdat8getIdTopEPh");
		util::ResolveSymbol<decltype(Bdat::getSheetName)>(&Bdat::getSheetName, "_ZN4Bdat12getSheetNameEPh");

		// Hook stuff
		Bdat::getMSTextHook();
	}

} // namespace bf2mods
