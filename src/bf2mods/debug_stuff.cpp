#include "debug_stuff.hpp"

#include <bf2mods/fw/debug.hpp>
#include <bf2mods/game/mapjump.hpp>
#include <bf2mods/game/scripts.hpp>
#include <bf2mods/gf/bgm.hpp>
#include <bf2mods/mm/math_types.hpp>
#include <bf2mods/stuff/utils/debug_util.hpp>
#include <bf2mods/tl/title.hpp>
#include <map>

#include "bf2logger.hpp"
#include "bf2mods/stuff/utils/util.hpp"
#include "bf2mods/utils.hpp"
#include "state.hpp"

#if BF2MODS_CODENAME(bfsw)
namespace game {

	GENERATE_SYM_HOOK(SeqUtil_requestMapJump, "_ZN4game7SeqUtil14requestMapJumpERKN2fw8DocumentERKNS_16MapJumpSetupInfoE", void, void* Document, MapJumpSetupInfo* sInfo) {
		SeqUtil_requestMapJumpBak(Document, sInfo);

		bf2mods::g_Logger->LogInfo("setup: %02u, %02u, %s, %s, %s", sInfo->chapter, sInfo->location, bf2mods::Prettyprinter<mm::Vec3>().format(sInfo->maybeVec).c_str(), bf2mods::Prettyprinter<mm::Vec3>().format(sInfo->jump_pos).c_str(), bf2mods::Prettyprinter<mm::Vec3>().format(sInfo->jump_rot).c_str());
	}

} // namespace game
#endif

namespace ml {

	namespace DrMdlMan {

		/*uint8_t(*headerChek)(const char* buffer);

		GENERATE_SYM_HOOK(createMdl, "_ZN2ml8DrMdlMan9createMdlEPKvPKci", void*, void* that, char* model_buffer, const char* model_path, int unknown) {
			bf2mods::g_Logger->LogInfo("Loading model \"%s\" - magic %c%c%c%c, chek %d", model_path, model_buffer[0], model_buffer[1], model_buffer[2], model_buffer[3], headerChek(model_buffer));
			return createMdlBak(that, model_buffer, model_path, unknown);
		}

		GENERATE_SYM_HOOK(createInstantMdl, "_ZN2ml8DrMdlMan16createInstantMdlEPKvPKci", void*, void* that, char* model_buffer, const char* model_path, int unknown) {
			bf2mods::g_Logger->LogInfo("Loading instant model \"%s\"", model_path);
			return createMdlBak(that, model_buffer, model_path, unknown);
		}*/

	}

} // namespace ml

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

namespace gf {

	GENERATE_SYM_HOOK(BgmTrack_update, "_ZN2gf8BgmTrack6updateERKN2fw10UpdateInfoE", void, gf::BgmTrack* this_pointer, void* updateInfo) {
		BgmTrack_updateBak(this_pointer, updateInfo);

		if(!bf2mods::GetState().options.enableDebugRendering)
			return;

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

		const int height = fw::debug::drawFontGetHeight();

		if(this_pointer->isPlaying()) {
			//mm::mtl::FixStr<64> fixStr;
			//memcpy(&fixStr.buffer, "asscafe", sizeof("asscafe"));
			//this_pointer->makePlayFileName(&fixStr);

			fw::debug::drawFont(0, 720 - (bf2mods::DebugStuff::bgmTrackIndex++ * height) - height, mm::Col4::White, "%s: %s %.1f/%.1f, looping: %s", trackName, this_pointer->getPlayingBgmFileName(), this_pointer->getPlayTime(), this_pointer->getTotalTime(), bf2mods::format(this_pointer->isLoop()).c_str());
		} else {
			// uncomment if you want every BgmTrack instance to show this
			//fw::debug::drawFont(0, 720 - (bf2mods::DebugStuff::bgmTrackIndex++ * height) - height, mm::Col4::White, "not playing");
		}
	}

	namespace GfPlayFactory {
		void (*createSkipTravel)(unsigned int mapjumpId);
	}

	namespace GfMenuObjUtil {
		int (*playSE)(SEIndex index);
	}

} // namespace gf

namespace nn {

	GENERATE_SYM_HOOK(hid_ShowControllerSupport, "_ZN2nn3hid21ShowControllerSupportEPNS0_27ControllerSupportResultInfoERKNS0_20ControllerSupportArgE", void, nn::hid::ControllerSupportResultInfo* resultInfo, nn::hid::ControllerSupportArg* supportArg) {
		nn::hid::NpadFullKeyState p1State {};
		nn::hid::GetNpadState(&p1State, nn::hid::CONTROLLER_PLAYER_1);

		if(p1State.Flags & nn::hid::NpadFlags::NPAD_CONNECTED) {
			// We have a player 1 at this point, so disable single-controller only mode
			// That way if another controller is connected while the game is running, we can allow it to be P2
			// Monolib actually passes the (according to switchbrew) default arguments to this,
			// so we technically get 4 max controllers for free!
			supportArg->mSingleMode = false;
		}

		hid_ShowControllerSupportBak(resultInfo, supportArg);
	}

} // namespace nn

namespace bf2mods::DebugStuff {

	int bgmTrackIndex = 0;

	void DoMapJump(unsigned int mapjumpId) {
#if !BF2MODS_CODENAME(bfsw)
		gf::GfPlayFactory::createSkipTravel(mapjumpId);
		gf::GfMenuObjUtil::playSE(gf::GfMenuObjUtil::SEIndex::mapjump);
#else
		game::MapJumpSetupInfo info;

		if(fw::document == nullptr) {
			g_Logger->LogError("can't do a map jump cause no doc ptr!");
			return;
		}

		g_Logger->LogInfo("going to make info");
		game::SeqUtil::makeMapJumpSetupInfoFromLandmark(info, *fw::document, mapjumpId);
		g_Logger->LogInfo("made info, going to request jump");
		game::SeqUtil::requestMapJump(*fw::document, info);
		g_Logger->LogInfo("jump requested");
#endif
	}

	void PlaySE(unsigned int soundEffect) {
#if !BF2MODS_CODENAME(bfsw)
		gf::GfMenuObjUtil::playSE((gf::GfMenuObjUtil::SEIndex)soundEffect);
#endif
	}
	void PlaySE(gf::GfMenuObjUtil::SEIndex soundEffect) {
#if !BF2MODS_CODENAME(bfsw)
		gf::GfMenuObjUtil::playSE(soundEffect);
#endif
	}

	void ReturnTitle(unsigned int slot) {
#if !BF2MODS_CODENAME(bfsw)
		tl::TitleMain::returnTitle((gf::SAVESLOT)slot);
#else
		if(fw::document == nullptr) {
			g_Logger->LogError("can't return to title cause no doc ptr!");
			return;
		}
		game::SeqUtil::returnTitle(*fw::document);
#endif
	}

	void Setup() {
		g_Logger->LogDebug("Setting up debug stuff...");

		mm::MMStdBase::mmAssertHook();

		//util::ResolveSymbol<decltype(ml::DrMdlMan::headerChek)>(&ml::DrMdlMan::headerChek, "_ZN2ml8DrMdlMan10headerChekEPKv");
		//ml::DrMdlMan::createMdlHook();
		//ml::DrMdlMan::createInstantMdlHook();

		nn::hid_ShowControllerSupportHook();

#if BF2MODS_CODENAME(bfsw)
		game::SeqUtil_requestMapJumpHook();
#else
		//event::MovieManager_makePathHook();
		gf::BgmTrack_updateHook();

		// Resolve some game framework symbols
		util::ResolveSymbol<decltype(gf::GfPlayFactory::createSkipTravel)>(&gf::GfPlayFactory::createSkipTravel, "_ZN2gf13GfPlayFactory16createSkipTravelEj");
		util::ResolveSymbol<decltype(gf::GfMenuObjUtil::playSE)>(&gf::GfMenuObjUtil::playSE, "_ZN2gf13GfMenuObjUtil6playSEEj");
#endif
	}

} // namespace bf2mods::DebugStuff
