#include "debug_stuff.hpp"

#include <bf2mods/engine/game/mapjump.hpp>
#include <bf2mods/engine/game/scripts.hpp>
#include <bf2mods/engine/gf/bgm.hpp>
#include <bf2mods/engine/gf/events.hpp>
#include <bf2mods/engine/gf/play_factory.hpp>
#include <bf2mods/engine/ml/debdraw.hpp>
#include <bf2mods/engine/mm/math_types.hpp>
#include <bf2mods/engine/tl/title.hpp>
#include <map>

#include "bf2logger.hpp"
#include "bf2mods/debug_wrappers.hpp"
#include "bf2mods/stuff/utils/debug_util.hpp"
#include "bf2mods/stuff/utils/util.hpp"
#include "bf2mods/utils.hpp"
#include "plugin_main.hpp"
#include "state.hpp"

#if BF2MODS_CODENAME(bfsw)
namespace game {

	GENERATE_SYM_HOOK(SeqUtil_requestMapJump, "_ZN4game7SeqUtil14requestMapJumpERKN2fw8DocumentERKNS_16MapJumpSetupInfoE", void, void* Document, MapJumpSetupInfo* sInfo) {
		SeqUtil_requestMapJumpBak(Document, sInfo);

		//bf2mods::g_Logger->LogInfo("setup: {:02u}, {:02u}, {}, {}, {}", sInfo->chapter, sInfo->location, sInfo->maybeVec, sInfo->jump_pos, sInfo->jump_rot);
	}

} // namespace game
#endif

namespace ml {

	namespace DrMdlMan {

		/*uint8_t(*headerChek)(const char* buffer);

		GENERATE_SYM_HOOK(createMdl, "_ZN2ml8DrMdlMan9createMdlEPKvPKci", void*, void* that, char* model_buffer, const char* model_path, int unknown) {
			bf2mods::g_Logger->LogInfo("Loading model \"{}\" - magic {:c}{:c}{:c}{:c}, chek {}", model_path, model_buffer[0], model_buffer[1], model_buffer[2], model_buffer[3], headerChek(model_buffer));
			return createMdlBak(that, model_buffer, model_path, unknown);
		}

		GENERATE_SYM_HOOK(createInstantMdl, "_ZN2ml8DrMdlMan16createInstantMdlEPKvPKci", void*, void* that, char* model_buffer, const char* model_path, int unknown) {
			bf2mods::g_Logger->LogInfo("Loading instant model \"{}\"", model_path);
			return createMdlBak(that, model_buffer, model_path, unknown);
		}*/

	}

	/*GENERATE_SYM_HOOK(FontLayer_font, "_ZN2ml9FontLayer4fontEiiPKcz", void, int x, int y, const char* format, ...) {
		__builtin_va_list val;
		__builtin_va_start(val, format);

		skyline::logger::s_Instance->LogFormat("ml::FontLayer::font: %s", format);
		FontLayer_fontBak(x, y, format, val);

		__builtin_va_end(val);
	}

	GENERATE_SYM_HOOK(DevFont_fontLayer, "_ZN2ml7DevFont9fontLayerEPKNS_9FontLayerEiiPKc", void, const void* fontLayer, int x, int y, const char* text) {

		skyline::logger::s_Instance->LogFormat("fontLayer: %s", text);
		dbgutil::logStackTrace();

		return DevFont_fontLayerBak(fontLayer, x, y, text);
	}*/

} // namespace ml

namespace mm {

	namespace MMStdBase {

		GENERATE_SYM_HOOK(mmAssert, "_ZN2mm9MMStdBase8mmAssertEPKcS2_j", void, const char* expr, const char* file, unsigned line) {
			bf2mods::g_Logger->LogFatal("Caught Assert!!! Expr \"{}\" ({} : {})", expr, file, line);
			mmAssertBak(expr, file, line);
		}

	} // namespace MMStdBase

} // namespace mm

namespace event {

	/*GENERATE_SYM_HOOK(MovieManager_makePath, "_ZN5event12MovieManager8makePathERN2mm3mtl6FixStrILi256EEEPKc", bool, mm::mtl::FixStr<256>* param_1, char* param_2) {
		//dbgutil::logStackTrace();

		bf2mods::g_Logger->LogInfo("param_1: {}, param_2: {}", &param_1->buffer, param_2);
		return MovieManager_makePathBak(param_1, param_2);
	}*/

	GENERATE_SYM_HOOK(Manager_play, "_ZN5event7Manager4playEPKcPN2gf13GF_OBJ_HANDLEEjjjjRKN2mm4Vec3Ef", void, void* this_pointer, const char* evtName, void* objHandle, uint param_3, uint param_4, uint param_5, uint param_6, mm::Vec3* playerPos, float param_8) {
		std::string eventName = evtName;
		if(eventName.size() > 10)
			eventName.resize(10);
		uint eventId = gf::GfDataEvent::getEventID(eventName.c_str());

		if(eventId > 0)
			bf2mods::g_Logger->LogDebug("Creating event {} (id {})", evtName, eventId);
		else
			bf2mods::g_Logger->LogDebug("Creating event {}", evtName);
		//bf2mods::g_Logger->LogDebug("Other args: {:p} {} {} {} {} {:2} {:2}", objHandle, param_3, param_4, param_5, param_6, static_cast<const glm::vec3&>(*playerPos), param_8);

		Manager_playBak(this_pointer, evtName, objHandle, param_3, param_4, param_5, param_6, playerPos, param_8);
	}

} // namespace event

namespace tl {

	GENERATE_SYM_HOOK(TitleMain_playTitleEvent, "_ZN2tl9TitleMain14playTitleEventEj", void, TitleMain* this_pointer, uint eventId) {
		uint newEventId = eventId;

		// get the clear count from the save because that's what everything else seems to do
		int clearCount = *reinterpret_cast<int*>(reinterpret_cast<char*>(this_pointer->getSaveBuffer()) + 0x109b3c);
		//bf2mods::g_Logger->LogDebug("chapter {}, clear count {}, needs cleared game? {}", this_pointer->getChapterIdFromSaveData(), clearCount, bf2mods::GetState().config.titleEventsNeedsClearedGame);

		// we need to have started the game (to avoid a lock) and optionally cleared the game once before
		// TODO: replace the global cleared game check with a per-event check
		// it would loop each thing in the array and exclude the events that one hasn't seen yet to avoid spoilers
		if(this_pointer->getChapterIdFromSaveData() > 0 && (clearCount > 0 || !bf2mods::GetState().config.titleEventsNeedsClearedGame)) {
			auto& events = bf2mods::GetState().config.titleEvents;

			if (!events.empty())
				newEventId = events[(util::nnRand<int16_t>() % events.size())];
		}

		if (newEventId != eventId)
			bf2mods::g_Logger->LogDebug("Replacing title event {} (id {}) with {} (id {})", gf::GfDataEvent::getEventName(eventId), eventId, gf::GfDataEvent::getEventName(newEventId), newEventId);
		TitleMain_playTitleEventBak(this_pointer, newEventId);
	}

}; // namespace tl

void (*__cxa_pure_virtual)();

namespace gf {

	GENERATE_SYM_HOOK(BgmTrack_update, "_ZN2gf8BgmTrack6updateERKN2fw10UpdateInfoE", void, gf::BgmTrack* this_pointer, void* updateInfo) {
		BgmTrack_updateBak(this_pointer, updateInfo);

		if(!bf2mods::DebugStuff::enableDebugRendering)
			return;

		auto* vtable = reinterpret_cast<BgmTrack::VfTable*>(*(size_t**)this_pointer);

		const int height = fw::debug::drawFontGetHeight();

		if(this_pointer->isPlaying()) {
			mm::mtl::FixStr<64> bgmFileName {};
			std::string trackName = "BgmTrack";

			if(!this_pointer->makePlayFileName(bgmFileName)) {
				// failed to make a filename, just fall back to playingBgmFileName
				bgmFileName.set(this_pointer->playingBgmFileName);
			}

			if(reinterpret_cast<void*>(&vtable->GetTrackName) != reinterpret_cast<void*>(&__cxa_pure_virtual)) {
				// not a pure virtual, so we can call this safely
				trackName = vtable->GetTrackName(this_pointer);
			}

			fw::debug::drawFontFmtShadow(0, 720 - (bf2mods::DebugStuff::bgmTrackIndex++ * height) - height, mm::Col4::White,
										 "{}: {} {:.1f}/{:.1f}, looping: {}", trackName, bgmFileName.buffer, this_pointer->getPlayTime(), this_pointer->getTotalTime(), this_pointer->isLoop());
		} else {
			// uncomment if you want every BgmTrack instance to show
			//fw::debug::drawFontFmtShadow(0, 720 - (bf2mods::DebugStuff::bgmTrackIndex++ * height) - height, mm::Col4::White, "{}: not playing", vtable->GetTrackName(this_pointer));
		}
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

namespace bf2mods {

	bool DebugStuff::enableDebugRendering = true;

	int DebugStuff::bgmTrackIndex = 0;

	void DebugStuff::DoMapJump(unsigned int mapjumpId) {
#if BF2MODS_CODENAME(bf2) || BF2MODS_CODENAME(ira)
		mapjumpId = std::clamp<unsigned>(mapjumpId, 1, 297);
#elif BF2MODS_CODENAME(bfsw)
		mapjumpId = std::clamp<unsigned>(mapjumpId, 1, 487);
#endif

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

	void DebugStuff::PlaySE(unsigned int soundEffect) {
#if !BF2MODS_CODENAME(bfsw)
		gf::GfMenuObjUtil::playSE(soundEffect);
#endif
	}

	void DebugStuff::ReturnTitle(unsigned int slot) {
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

	void DebugStuff::Initialize() {
		g_Logger->LogDebug("Setting up debug stuff...");

		mm::MMStdBase::mmAssertHook();

		//util::ResolveSymbol<decltype(ml::DrMdlMan::headerChek)>(&ml::DrMdlMan::headerChek, "_ZN2ml8DrMdlMan10headerChekEPKv");
		//ml::DrMdlMan::createMdlHook();
		//ml::DrMdlMan::createInstantMdlHook();
		//ml::FontLayer_fontHook();
		//ml::DevFont_fontLayerHook();

		nn::hid_ShowControllerSupportHook();

#if BF2MODS_CODENAME(bfsw)
		game::SeqUtil_requestMapJumpHook();
#else
		//event::MovieManager_makePathHook();

		util::ResolveSymbol<decltype(__cxa_pure_virtual)>(&__cxa_pure_virtual, "__cxa_pure_virtual");

		gf::BgmTrack_updateHook();

		tl::TitleMain_playTitleEventHook();
		event::Manager_playHook();
#endif
	}

	void DebugStuff::Update() {
		auto& state = GetState();

		bgmTrackIndex = 0;

		if(btnDown(Keybind::DEBUG_RENDER_TOGGLE, p2Cur.Buttons, p2Prev.Buttons)) {
			enableDebugRendering = !enableDebugRendering;
#if !BF2MODS_CODENAME(bfsw)
			fw::PadManager::enableDebugDraw(enableDebugRendering);
#endif
			g_Logger->LogInfo("Debug rendering: {}", enableDebugRendering);
		}

		else if(btnDown(Keybind::TEMPINT_INC, p2Cur.Buttons, p2Prev.Buttons)) {
			state.tempInt++;
			g_Logger->LogInfo("TempInt++, now {}", state.tempInt);
		} else if(btnDown(Keybind::TEMPINT_DEC, p2Cur.Buttons, p2Prev.Buttons)) {
			state.tempInt--;
			g_Logger->LogInfo("TempInt--, now {}", state.tempInt);
		} else if(btnDown(Keybind::MAPJUMP_JUMP, p2Cur.Buttons, p2Prev.Buttons)) {
			g_Logger->LogInfo("Attempting jump to MapJump {}", state.tempInt);
			DoMapJump(state.tempInt);
		} else if(btnDown(Keybind::PLAYSE, p2Cur.Buttons, p2Prev.Buttons)) {
			g_Logger->LogInfo("Sound effect {} (0x{:x})", state.tempInt, state.tempInt);
			PlaySE(state.tempInt);
		}

		else if(btnDown(Keybind::RETURN_TO_TITLE, p2Cur.Buttons, p2Prev.Buttons)) {
			PlaySE(gf::GfMenuObjUtil::SEIndex::Sort);
			ReturnTitle(-1);
		}
	}

	BF2MODS_REGISTER_MODULE(DebugStuff);

} // namespace bf2mods