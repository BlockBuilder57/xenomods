#include "debug_stuff.hpp"


#include "bf2mods/stuff/utils/util.hpp"
#include "skyline/logger/Logger.hpp"

namespace bf2mods {

	namespace gf {

		namespace GfManager {

			GENERATE_SYM_HOOK(isExecModeTool, "_ZN2gf9GfManager14isExecModeToolEv", bool, void* _this) {
				return true;
			}

			GENERATE_SYM_HOOK(isExecModeDebug, "_ZN2gf9GfManager15isExecModeDebugEv", bool, void* _this) {
				return true;
			}

		} // namespace GfManager

		namespace GfDataMap {

			// to be documented
			std::uint64_t (*getMapJumpID)(unsigned n);
			/*std::uint64_t(*getMapJumpID)(unsigned n);
		std::uint64_t(*getMapID)(unsigned n);*/

			GENERATE_SYM_HOOK(getMapID, "_ZN2gf9GfDataMap8getMapIDEj", std::uint64_t, unsigned int n) {
				constexpr auto _override = 30ul;
				skyline::logger::s_Instance->LogFormat("gf::GfDataMap::getMapID(n: %u), just gonna return \"%llu\"", n, _override);
				return _override;
			}

		} // namespace GfDataMap

		namespace GfBdatMaplist {

			GENERATE_SYM_HOOK(getMaplistresource, "_ZNK2gf13GfBdatMaplist18getMaplistresourceEj", char*, void* _this, unsigned int n) {
				constexpr char* override_str = "ma30a";
				skyline::logger::s_Instance->LogFormat("gf::GfBdatMaplist::getMaplistresource(this: %p, n: %u), just gonna return \"%s\"", _this, n, override_str);
				return override_str;
			}

		} // namespace GfBdatMaplist

		namespace GfGameUtilSkipTravel {

			GENERATE_SYM_HOOK(isEnableSkipTravel, "_ZN2gf20GfGameUtilSkipTravel18isEnableSkipTravelEj", bool, unsigned int n) {
				skyline::logger::s_Instance->LogFormat("gf::GfGameUtilSkipTravel::isEnableSkipTravel(n: 0x%08x), just gonna return true", n);
				skyline::logger::s_Instance->LogFormat("\t\b\b\b\bMapID: 0x%08x", GfDataMap::getMapIDReplace(n));
				skyline::logger::s_Instance->LogFormat("\t\b\b\b\bMapJumpID: 0x%08x", GfDataMap::getMapJumpID(n));

				return true;
			}

		} // namespace GfGameUtilSkipTravel

	} // namespace gf

	void SetupDebugStuff() {
		skyline::logger::s_Instance->LogFormat("Setting up debug stuff...");

		// hook both of these to always be true
		gf::GfManager::isExecModeToolHook();
		gf::GfManager::isExecModeDebugHook();

		// Resolve some game framework symbols
		util::ResolveSymbol<decltype(gf::GfDataMap::getMapJumpID)>(&gf::GfDataMap::getMapJumpID, "_ZN2gf9GfDataMap12getMapJumpIDEj");
		//util::ResolveSymbol<decltype(gf::GfDataMap::getMapID)>(&gf::GfDataMap::getMapID, "_ZN2gf9GfDataMap8getMapIDEj");

		// Hook a GF symbol cause why not:tm:
		//gf::GfDataMap::getMapIDHook();

		// this one is hacky but i do not give a rats fucking cock
		//gf::GfBdatMaplist::getMaplistresourceHook();

		gf::GfGameUtilSkipTravel::isEnableSkipTravelHook();
	}

} // namespace bf2mods
