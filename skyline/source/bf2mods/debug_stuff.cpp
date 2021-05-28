#include "debug_stuff.hpp"

#include "bf2mods/stuff/utils/util.hpp"
#include "bf2mods/utils.hpp"
#include "skyline/logger/Logger.hpp"

//#include <bf2mods/prettyprinter.hpp>

#include <bf2mods/mm/math_types.hpp>
#include <bf2mods/stuff/utils/debug_util.hpp>

namespace bf2mods {

	namespace ml {

		GENERATE_SYM_HOOK(isWinDestroy, "_ZN2ml8WorkRoot12isWinDestroyEv", bool) {
			static bool called_once_by_game = false;
			if(!called_once_by_game) {
				called_once_by_game = true;
				return isWinDestroyBak();
			}

			return true;
		}

	} // namespace ml

	namespace fw::debug {

		void (*drawFont)(int x, int y, mm::Col4* color, const char* fmt, ...);

	}

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

			// this hook will force the game to always load map data for the a given map

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

		struct GfMoveJumpControler {
			struct Packet { // we only care abt stuff here
				mm::Vec3 velSomething;
			};
		};

		GENERATE_SYM_HOOK(GfMoveJumpControler_calcAngularVelocity, "_ZN2gf19GfMoveJumpControler19calcAngularVelocityEfRNS0_6PacketE", void, GfMoveJumpControler* that, float f, GfMoveJumpControler::Packet* packetRef) {
			//skyline::logger::s_Instance->LogFormat("gf::GfMoveJumpControler::calcAngularVelocity(this: %p)", that);
			//skyline::logger::s_Instance->LogFormat("DEBUG packetRef->velSomething:\n%s\n", bf2mods::format_with_typename<mm::Vec3>(packetRef->velSomething).c_str());

			// this *would* draw debug text if it worked
			fw::debug::drawFont(299, 299, &mm::Col4_Black, "packetRef->velSomething: %s", bf2mods::format_with_typename<mm::Vec3>(packetRef->velSomething).c_str());
			fw::debug::drawFont(300, 300, &mm::Col4_White, "packetRef->velSomething: %s", bf2mods::format_with_typename<mm::Vec3>(packetRef->velSomething).c_str());

			//dbgutil::logStackTrace();

			// call the original
			GfMoveJumpControler_calcAngularVelocityBak(that, f, packetRef);
			packetRef->velSomething.y += 32.f;

			// limit individual value
			auto limit_value = [](float& value) {
				constexpr auto limit = 200.f;

				if(value > limit)
					value = limit;

				if(value < -limit)
					value = -limit;
			};

			//limit_value(packetRef->velSomething.x);
			limit_value(packetRef->velSomething.y);
			//limit_value(packetRef->velSomething.z);
		}

	} // namespace gf

	/*
	namespace btl {

		struct BattleCharacter {
			INSERT_PADDING_BYTES(0x20)
			mm::Vec3 some_pos;
		};
	
	}
	*/

	void SetupDebugStuff() {
		skyline::logger::s_Instance->LogFormat("Setting up debug stuff...");

		// hook both of these to always be true
		gf::GfManager::isExecModeToolHook();
		gf::GfManager::isExecModeDebugHook();

		// Resolve some game framework symbols
		util::ResolveSymbol<decltype(gf::GfDataMap::getMapJumpID)>(&gf::GfDataMap::getMapJumpID, "_ZN2gf9GfDataMap12getMapJumpIDEj");
		//util::ResolveSymbol<decltype(gf::GfDataMap::getMapID)>(&gf::GfDataMap::getMapID, "_ZN2gf9GfDataMap8getMapIDEj");

		// oh god
		util::ResolveSymbol<decltype(fw::debug::drawFont)>(&fw::debug::drawFont, "_ZN2fw5debug8drawFontEiiPKcz");

		ml::isWinDestroyHook();

		// Hook a GF symbol cause why not:tm:
		//gf::GfDataMap::getMapIDHook();

		// this one is hacky but i do not give a rats fucking cock
		//gf::GfBdatMaplist::getMaplistresourceHook();

		gf::GfGameUtilSkipTravel::isEnableSkipTravelHook();

		gf::GfMoveJumpControler_calcAngularVelocityHook();
	}

} // namespace bf2mods
