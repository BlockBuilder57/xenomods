#include "party_selector.hpp"


#include "bf2mods/stuff/utils/util.hpp"
#include "skyline/logger/Logger.hpp"

namespace bf2mods {

	void hook_party_selector() {
		skyline::logger::s_Instance->LogFormat("Setting up party selector...");

		// Hook stuff
		//getMSTextHook();
	}

} // namespace bf2mods
