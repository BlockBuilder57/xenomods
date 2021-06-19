#include "bdat_randomizer.hpp"
#include "plugin.hpp"

#include "nn/oe.h"
#include "bf2mods/stuff/utils/util.hpp"
#include "skyline/logger/Logger.hpp"

#include <version.h>

namespace bf2mods {

	// use this if you want your game to be in 6th grader translation
	//constexpr char* PossibleThings[] = {
	//	"jeff",
	//	"ligma",
	//	"sugmy"
	//};

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
			const char* sheetName;
			const char* message;

			sheetName = Bdat::getSheetName(bdatData);

			if (strcmp(sheetName, "menu_ms") == 0) {
				// I don't like hardcoding these but they don't seem to change between versions
				// it's unfortunately nicer (imo) than doing a string comparison
				if (n == 1830) {
					//nn::oe::DisplayVersion displayVersion;
					//nn::oe::GetDisplayVersion(&displayVersion);

					std::stringstream ss;
					ss << "(" << version::tagDirty << ") ";

					return ss.str().c_str();
				}
				else if (n == 1610) {
					// it says "Loading" in the japanese version too so I'm not allowed to moan about hardcoding this
					return "Loading (modded)";
				}
			}

			switch (Plugin::getSharedStatePtr()->options.bdat.scrambleType) {
				case SharedState::Options::BdatOptions::ScrambleType::ScrambleIndex:
					// scrambles the index of the ms text sheet
					message = Bdat::getMSTextBak(bdatData, (util::nnRand<int16_t>() % Bdat::getIdCount(bdatData)) + Bdat::getIdTop(bdatData));
					break;
				case SharedState::Options::BdatOptions::ScrambleType::ShowSheetName:
					message = sheetName;
					break;
				case SharedState::Options::BdatOptions::ScrambleType::Off:
				default:
					message = Bdat::getMSTextBak(bdatData, n);
					break;
			}

			//skyline::logger::s_Instance->LogFormat("Bdat %d overridden to %d", n, new_n);

			//skyline::logger::s_Instance->LogFormat("Uhhhh %p %p", Bdat::getIdCount, Bdat::getIdTop);
			//skyline::logger::s_Instance->LogFormat("For %p: Count = %d, Top = %d", bdatData,  Bdat::getIdCount(bdatData), Bdat::getIdTop(bdatData));

			// 6th grader part 2
			//return PossibleThings[rand() % (sizeof(PossibleThings)/sizeof(char*))];
			return message;
		}

		// todo more stuff to randomize more things
	} // namespace Bdat

	void SetupBdatRandomizer() {
		skyline::logger::s_Instance->LogFormat("Setting up BDAT randomizer...");

		// resolve any engine symbols we need (would be nice to have a static function object do that but meh)
		skyline::logger::s_Instance->LogFormat("Resolving Bdat:: symbols...");

		util::ResolveSymbol<decltype(Bdat::getIdCount)>(&Bdat::getIdCount, "_ZN4Bdat10getIdCountEPh");
		util::ResolveSymbol<decltype(Bdat::getIdTop)>(&Bdat::getIdTop, "_ZN4Bdat8getIdTopEPh");
		util::ResolveSymbol<decltype(Bdat::getSheetName)>(&Bdat::getSheetName, "_ZN4Bdat12getSheetNameEPh");

		skyline::logger::s_Instance->LogFormat("Finished resolving Bdat:: symbols");

		//skyline::logger::s_Instance->LogFormat("Uhhhh %p %p", Bdat::getIdCount, Bdat::getIdTop);

		// Hook stuff
		Bdat::getMSTextHook();
	}

} // namespace bf2mods
