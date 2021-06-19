#include "debug_stuff.hpp"

#include "bf2logger.hpp"
#include "bf2mods/stuff/utils/util.hpp"
#include "bf2mods/utils.hpp"
#include "plugin.hpp"
#include "skyline/logger/Logger.hpp"

//#include <bf2mods/prettyprinter.hpp>

#include <bf2mods/mm/math_types.hpp>
#include <bf2mods/stuff/utils/debug_util.hpp>
#include <bf2mods/gf/bgm.hpp>
#include <map>

void(* cxa_pure_virtual)();

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
			bf2mods::bgmTrackIndex = 0;
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

namespace event {

	/*GENERATE_SYM_HOOK(MovieManager_makePath, "_ZN5event12MovieManager8makePathERN2mm3mtl6FixStrILi256EEEPKc", bool, mm::mtl::FixStr<256>* param_1, char* param_2) {
		//dbgutil::logStackTrace();

		bf2mods::g_Logger->LogInfo("param_1: %s, param_2: %s", &param_1->buffer, param_2);
		return MovieManager_makePathBak(param_1, param_2);
	}*/

} // namespace event

template<class T, class Y>
constexpr T ub_cast(Y y) {
	union {
		T t;
		Y y;
	} u { .y = y };

	return u.t;
}

namespace gf {

	GENERATE_SYM_HOOK(BgmTrack_update, "_ZN2gf8BgmTrack6updateERKN2fw10UpdateInfoE", void, gf::BgmTrack* this_pointer, void* updateInfo) {
		BgmTrack_updateBak(this_pointer, updateInfo);

		char trackName[32];
		memset(&trackName, 0, sizeof(trackName));
		memcpy(&trackName, "BgmTrack", sizeof("BgmTrack"));

		// Oh no help

		//bf2mods::g_Logger->LogInfo("virtual: %p", cxa_pure_virtual);
		//bf2mods::g_Logger->LogInfo("track name: %p", ub_cast<void*>(this_pointer->vtable->GetTrackName));

		/*if(ub_cast<void*>(this_pointer->vtable->GetTrackName) != cxa_pure_virtual) {
			bf2mods::g_Logger->LogInfo("GetTrackName isn't pure virtual..");
			const char* barack_obama = (this_pointer->*(this_pointer->vtable->GetTrackName))();
			bf2mods::g_Logger->LogInfo("it's %s", barack_obama);
		}*/

		if (this_pointer->isPlaying()) {
			//mm::mtl::FixStr<64> fixStr;
			//memcpy(&fixStr.buffer, "asscafe", sizeof("asscafe"));
			//this_pointer->makePlayFileName(&fixStr);

			fw::debug::drawFont(0, (720) - (bf2mods::bgmTrackIndex++ * 16) - 16, &mm::Col4::White, "%s: %s %.1f/%.1f, looping: %s", trackName, this_pointer->getPlayingBgmFileName() , this_pointer->getPlayTime(), this_pointer->getTotalTime(), bf2mods::format(this_pointer->isLoop()).c_str());
		}
		else {
			//fw::debug::drawFont(1280/2, (720/2) + (bf2mods::bgmTrackTest++ * 16), &mm::Col4::White, "not playing");
		}
	}

	namespace GfPlayFactory {
		void (*createSkipTravel)(unsigned int mapjumpId);
	}

	namespace GfMenuObjUtil {
		int (*playSE)(SEIndex index);
	}

} // namespace gf

namespace bf2mods {

	int bgmTrackIndex = 0;

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

		//util::ResolveSymbol<void(*)()>(&cxa_pure_virtual, "__cxa_pure_virtual");

		/*uintptr_t cxa_pure_virtual;

		if (R_SUCCEEDED(nn::ro::LookupSymbol(&cxa_pure_virtual, "__cxa_pure_virtual"))) {
			g_Logger->LogWarning("we're good");
		} else {
			g_Logger->LogFatal("NOT FINE NOT FINE");
		}*/

		g_Logger->LogMessage(Logger::Severity::Info, "Setting up debug stuff...");

		util::ResolveSymbol<decltype(fw::PadManager::enableDebugDraw)>(&fw::PadManager::enableDebugDraw, "_ZN2fw10PadManager15enableDebugDrawEb");
		fw::PadManager::updateHook();

		//event::MovieManager_makePathHook();
		gf::BgmTrack_updateHook();

		// Resolve some game framework symbols
		util::ResolveSymbol<decltype(gf::GfPlayFactory::createSkipTravel)>(&gf::GfPlayFactory::createSkipTravel, "_ZN2gf13GfPlayFactory16createSkipTravelEj");
		util::ResolveSymbol<decltype(gf::GfMenuObjUtil::playSE)>(&gf::GfMenuObjUtil::playSE, "_ZN2gf13GfMenuObjUtil6playSEEj");
	}

} // namespace bf2mods
