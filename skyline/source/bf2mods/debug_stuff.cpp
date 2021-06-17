#include "debug_stuff.hpp"

#include "bf2logger.hpp"
#include "bf2mods/stuff/utils/util.hpp"
#include "bf2mods/utils.hpp"
#include "plugin.hpp"
#include "skyline/logger/Logger.hpp"

//#include <bf2mods/prettyprinter.hpp>

#include <bf2mods/gf/misc.h>

#include <bf2mods/mm/math_types.hpp>
#include <bf2mods/stuff/utils/debug_util.hpp>
#include <map>

namespace fw {

	namespace debug {

		bool (*drawAxis)(const mm::Mat44* transform, float scale);
		bool (*drawFont)(int x, int y, const mm::Col4* color, const char* fmt, ...);
		int (*drawFontGetWidth)(const char* fmt, ...);
		int (*drawFontGetHeight)(const char* fmt, ...);
		void (*drawCompareZ)(bool compare);

	}

	namespace PadManager {

		void (*enableDebugDraw)(bool enable);

		GENERATE_SYM_HOOK(update, "_ZN2fw10PadManager6updateERKNS_10UpdateInfoE", void, void* this_pointer, void* updateInfo) {
			updateBak(this_pointer, updateInfo);
			enableDebugDraw(true);
		}

	}

} // namespace fw

namespace mm {

	namespace MMStdBase {

		GENERATE_SYM_HOOK(mmAssert, "_ZN2mm9MMStdBase8mmAssertEPKcS2_j", void, const char* expr, const char* file, unsigned line) {
			bf2mods::g_Logger->LogFatal("Caught Assert!!! Expr \"%s\" (%s : %d)", expr, file, line);
			mmAssertBak(expr, file, line);
		}

	} // namespace MMStdBase

} // namespace mm

namespace gf {

	namespace GfPlayFactory {
		void (*createSkipTravel)(unsigned int mapjumpId);
	}

	namespace GfMenuObjUtil {
		int (*playSE)(SEIndex index);
	}

} // namespace gf

namespace bf2mods {

	void DoMapJump(unsigned int mapjumpId) {
		gf::GfPlayFactory::createSkipTravel(mapjumpId);
		gf::GfMenuObjUtil::playSE(gf::GfMenuObjUtil::SEIndex::mapjump);
	}

	void PlaySE(unsigned int soundEffect) {
		gf::GfMenuObjUtil::playSE((gf::GfMenuObjUtil::SEIndex)soundEffect);
	}
	void PlaySE(gf::GfMenuObjUtil::SEIndex soundEffect) {
		gf::GfMenuObjUtil::playSE(soundEffect);
	}

	void SetupDebugStuff() {
		mm::MMStdBase::mmAssertHook();

		// Load debug draw functions. Once this is setup the g_Logger can be used
		// !!!!!! TODO !!!!!! MOVE THIS INTO SEPARATE FILES PER SUBSYSTEM
		util::ResolveSymbol<decltype(fw::debug::drawAxis)>(&fw::debug::drawAxis, "_ZN2fw5debug8drawAxisERKN2mm5Mat44Ef");
		util::ResolveSymbol<decltype(fw::debug::drawFont)>(&fw::debug::drawFont, "_ZN2fw5debug8drawFontEiiRKN2mm4Col4EPKcz");
		util::ResolveSymbol<decltype(fw::debug::drawFontGetWidth)>(&fw::debug::drawFontGetWidth, "_ZN2fw5debug16drawFontGetWidthEPKcz");
		util::ResolveSymbol<decltype(fw::debug::drawFontGetHeight)>(&fw::debug::drawFontGetHeight, "_ZN2fw5debug17drawFontGetHeightEv");
		util::ResolveSymbol<decltype(fw::debug::drawCompareZ)>(&fw::debug::drawCompareZ, "_ZN2fw5debug12drawCompareZEb");

		g_Logger->LogMessage(Logger::Severity::Info, "Setting up debug stuff...");

		util::ResolveSymbol<decltype(fw::PadManager::enableDebugDraw)>(&fw::PadManager::enableDebugDraw, "_ZN2fw10PadManager15enableDebugDrawEb");
		fw::PadManager::updateHook();

		// Resolve some game framework symbols
		util::ResolveSymbol<decltype(gf::GfPlayFactory::createSkipTravel)>(&gf::GfPlayFactory::createSkipTravel, "_ZN2gf13GfPlayFactory16createSkipTravelEj");
		util::ResolveSymbol<decltype(gf::GfMenuObjUtil::playSE)>(&gf::GfMenuObjUtil::playSE, "_ZN2gf13GfMenuObjUtil6playSEEj");
	}

} // namespace bf2mods
