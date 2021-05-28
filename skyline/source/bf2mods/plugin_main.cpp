#include "plugin.hpp"
#include "skyline/efl/service.hpp"

// all parts here
#include "bdat_randomizer.hpp"
#include "debug_stuff.hpp"
#include "test.hpp"

namespace bf2mods {

	void SetupEverything() {
		Plugin::init();

		// hook stuff
		SetupBdatRandomizer();
		SetupDebugStuff();

		// lazy
		testhook();

		// R_ERRORONFAIL(skyline::efl::RegisterPlugin(Plugin::getPluginMeta()));
		// R_ERRORONFAIL(skyline::efl::RegisterSharedMem(Plugin::PLUGIN_NAME, Plugin::getPluginSharedMemInfo()));
	}

} // namespace bf2mods
