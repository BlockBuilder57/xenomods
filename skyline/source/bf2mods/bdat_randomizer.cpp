#include "bdat_randomizer.hpp"

#include "bf2mods/stuff/utils/util.hpp"
#include "skyline/logger/Logger.hpp"

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

		GENERATE_SYM_HOOK(getMSText, "_ZN4Bdat9getMSTextEPhi", char*, std::uint8_t* bdatData, int n) {
			// skyline::logger::s_Instance->LogFormat("Bdat::getMSText(bdat: %p, n: %d)", bdatData, n);

			int new_n = (util::nnRand<int16_t>() % Bdat::getIdCount(bdatData)) + Bdat::getIdTop(bdatData);
			auto message = Bdat::getMSTextBak(bdatData, new_n);

			//skyline::logger::s_Instance->LogFormat("Bdat %d overridden to %d", n, new_n);

			//skyline::logger::s_Instance->LogFormat("Uhhhh %p %p", Bdat::getIdCount, Bdat::getIdTop);
			//skyline::logger::s_Instance->LogFormat("For %p: Count = %d, Top = %d", bdatData,  Bdat::getIdCount(bdatData), Bdat::getIdTop(bdatData));

			// ditto ^^ comment
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

		skyline::logger::s_Instance->LogFormat("Finished resolving Bdat:: symbols");

		//skyline::logger::s_Instance->LogFormat("Uhhhh %p %p", Bdat::getIdCount, Bdat::getIdTop);

		// Hook stuff
		Bdat::getMSTextHook();
	}

} // namespace bf2mods
