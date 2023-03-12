#include "DebugStuff.hpp"

#include <xenomods/DebugWrappers.hpp>
#include <xenomods/HidInput.hpp>
#include <xenomods/Logger.hpp>
#include <xenomods/Utils.hpp>
#include <skylaunch/hookng/Hooks.hpp>

#include "../State.hpp"
#include "xenomods/engine/bdat/Bdat.hpp"
#include "xenomods/engine/fw/Debug.hpp"
#include "xenomods/engine/fw/Framework.hpp"
#include "xenomods/engine/fw/UpdateInfo.hpp"
#include "xenomods/engine/game/DebugDraw.hpp"
#include "xenomods/engine/game/MapJump.hpp"
#include "xenomods/engine/gf/BdatData.hpp"
#include "xenomods/engine/gf/Bgm.hpp"
#include "xenomods/engine/gf/MenuObject.hpp"
#include "xenomods/engine/gf/PlayFactory.hpp"
#include "xenomods/engine/ml/DebugDrawing.hpp"
#include "xenomods/engine/ml/Rand.hpp"
#include "xenomods/engine/ml/Scene.hpp"
#include "xenomods/engine/ml/WinView.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"
#include "xenomods/engine/tl/title.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"
#include "xenomods/stuff/utils/util.hpp"

namespace {

	struct MMAssert : skylaunch::hook::Trampoline<MMAssert> {
		static void Hook(const char* expr, const char* file, unsigned line) {
			xenomods::g_Logger->LogFatal("Caught Assert!!! Expr \"{}\" ({} : {})", expr, file, line);
			Orig(expr, file, line);
		}
	};

	void PrintEventInfo(const char* evtName) {
		std::string eventName = evtName;
		if(eventName.size() > 10)
			eventName.resize(10);
		uint eventId = gf::GfDataEvent::getEventID(eventName.c_str());

		if(eventId > 0)
			xenomods::g_Logger->LogDebug("Creating event {} (id {})", evtName, eventId);
		else
			xenomods::g_Logger->LogDebug("Creating event {}", evtName);
	}

	struct EventStartInfo : skylaunch::hook::Trampoline<EventStartInfo> {
		static void Hook(void* this_pointer, const char* evtName, void* objHandle, uint param_3, uint param_4, uint param_5, uint param_6, mm::Vec3* playerPos, float param_8) {
			PrintEventInfo(evtName);
			Orig(this_pointer, evtName, objHandle, param_3, param_4, param_5, param_6, playerPos, param_8);
		}
	};

	struct EventStartInfo_Earlier : skylaunch::hook::Trampoline<EventStartInfo_Earlier> {
		static void Hook(void* this_pointer, const char* evtName, void* objHandle, uint param_3, uint param_4, uint param_5, uint param_6) {
			PrintEventInfo(evtName);
			Orig(this_pointer, evtName, objHandle, param_3, param_4, param_5, param_6);
		}
	};

	struct ReplaceTitleEvent : skylaunch::hook::Trampoline<ReplaceTitleEvent> {
		static void Hook(tl::TitleMain* this_pointer, uint eventId) {
			uint newEventId = eventId;

			// no randomizing new games!! only allow the intended call through
			auto stack = dbgutil::getStackTrace();
			// ow my soul, find a better way to do this
			if (dbgutil::getSymbol(stack[1], true) != "_ZN2tl20TitleStateMainScreen6updateEPNS_9TitleMainERKN2fw10UpdateInfoE") {
				xenomods::g_Logger->LogDebug("Not replacing title event {} (id {}) as it would cause a lock (called by {})", gf::GfDataEvent::getEventName(eventId), eventId, dbgutil::getSymbol(stack[1]));
				return Orig(this_pointer, eventId);
			}

			// get the clear count from the save because that's what everything else seems to do
			uint clearCount = *reinterpret_cast<uint*>(reinterpret_cast<char*>(this_pointer->getSaveBuffer()) + 0x109b3c);
			uint chapter = this_pointer->getChapterIdFromSaveData();
#if XENOMODS_CODENAME(ira)
			chapter = 0; // it's always 10!
#endif
			chapter |= clearCount << 16; // "encodes" as 0x00010006 for 1 clear on ch6

			//xenomods::g_Logger->LogDebug("Chapter info: {:#x}", chapter);

			// we need to have started the game, as the opening cutscene just continues off the titlescreen
			if(chapter > 0) {
				auto& events = xenomods::GetState().config.titleEvents;

				if(!events.empty()) {
					const auto eventsDefault = std::vector<uint16_t>(CONFIG_TITLEEVENTS_DEFAULT);
					if (events == eventsDefault && chapter <= 10) {
						// we're still the default, so let's be fancy and progressively reveal chapters
						// once the user clears the game at least once this path will never be run,
						// so the extra stuff in the default (post-credits titlescreens) won't be shown
						newEventId = events[(ml::mtRand() % (chapter + 1))]; // +1 for the initial titlescreen
					}
					else
						newEventId = events[(ml::mtRand() % events.size())];
				}
			}

			if(newEventId != eventId)
				xenomods::g_Logger->LogDebug("Replacing title event {} (id {}) with {} (id {})", gf::GfDataEvent::getEventName(eventId), eventId, gf::GfDataEvent::getEventName(newEventId), newEventId);
			Orig(this_pointer, newEventId);
		}
	};

	struct BGMDebugging : skylaunch::hook::Trampoline<BGMDebugging> {
		static void Hook(gf::BgmTrack* this_pointer, fw::UpdateInfo* updateInfo) {
			Orig(this_pointer, updateInfo);

			if(!xenomods::DebugStuff::enableDebugRendering)
				return;

			const int height = fw::debug::drawFontGetHeight();
			std::string trackName = this_pointer->getTrackName();

			if (trackName == "EventBGM")
				return; // already shown by event::BgmManager

			if(this_pointer->isPlaying()) {
				mm::mtl::FixStr<64> bgmFileName {};

				if(!this_pointer->makePlayFileName(bgmFileName)) {
					// failed to make a filename, just fall back to playingBgmFileName
					bgmFileName.set(this_pointer->playingBgmFileName);
				}

				fw::debug::drawFontFmtShadow(0, 720 - (xenomods::DebugStuff::bgmTrackIndex++ * height) - height, mm::Col4::white,
											 "{}: {} {:.1f}/{:.1f}{}", trackName, bgmFileName.buffer, this_pointer->getPlayTime(), this_pointer->getTotalTime(), this_pointer->isLoop() ? " (∞)" : "");
			} else {
				// uncomment if you want every BgmTrack instance to show
				//fw::debug::drawFontFmtShadow(0, 720 - (xenomods::DebugStuff::bgmTrackIndex++ * height) - height, mm::Col4::white, "{}: not playing", trackName);
			}
		}
	};

	struct JumpToClosedLandmarks_World : skylaunch::hook::Trampoline<JumpToClosedLandmarks_World> {
		static bool Hook(unsigned int mapjump) {
			return xenomods::DebugStuff::accessClosedLandmarks ? true : Orig(mapjump);
		}
	};

	struct JumpToClosedLandmarks_Zone : skylaunch::hook::Trampoline<JumpToClosedLandmarks_Zone> {
		static bool Hook(unsigned int mapjump) {
			return xenomods::DebugStuff::accessClosedLandmarks ? true : Orig(mapjump);
		}
	};

#if XENOMODS_CODENAME(bfsw)
	static unsigned int ForceWinID = 0;

	struct GetWinIDOverride : skylaunch::hook::Trampoline<GetWinIDOverride> {
		static unsigned int Hook() {
			return ForceWinID;
		}
	};

	struct RenderViewHook : skylaunch::hook::Trampoline<RenderViewHook> {
		static void Hook(ml::WinView* this_pointer) {
			ForceWinID = this_pointer->windowID;
			//xenomods::g_Logger->LogInfo("winview {} winid {} equal? {}", this_pointer->windowID, ml::DebDraw::getCacheDrawWID(), this_pointer->windowID == ml::DebDraw::getCacheDrawWID());
			Orig(this_pointer);
		}
	};
#endif

}

namespace xenomods {

	bool DebugStuff::enableDebugRendering = true;
	bool DebugStuff::accessClosedLandmarks = true;

	int DebugStuff::bgmTrackIndex = 0;

	void DebugStuff::DoMapJump(unsigned int mapjumpId) {
#if XENOMODS_CODENAME(bf2) || XENOMODS_CODENAME(ira)
		mapjumpId = std::clamp<unsigned>(mapjumpId, 1, 297);
#elif XENOMODS_CODENAME(bfsw)
		mapjumpId = std::clamp<unsigned>(mapjumpId, 1, 487);
#endif

#if !XENOMODS_CODENAME(bfsw)
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
#if !XENOMODS_CODENAME(bfsw)
		gf::GfMenuObjUtil::playSE(soundEffect);
#endif
	}

	void DebugStuff::ReturnTitle(unsigned int slot) {
#if !XENOMODS_CODENAME(bfsw)
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

		MMAssert::HookAt("_ZN2mm9MMStdBase8mmAssertEPKcS2_j");

#if !XENOMODS_CODENAME(bfsw)
		BGMDebugging::HookAt("_ZN2gf8BgmTrack6updateERKN2fw10UpdateInfoE");

		// earlier versions didn't include the last two parameters
		if (skylaunch::hook::detail::ResolveSymbolBase("_ZN5event7Manager4playEPKcPN2gf13GF_OBJ_HANDLEEjjjjRKN2mm4Vec3Ef") == 0xDEADDEAD)
			EventStartInfo_Earlier::HookAt("_ZN5event7Manager4playEPKcPN2gf13GF_OBJ_HANDLEEjjjj");
		else
			EventStartInfo::HookAt("_ZN5event7Manager4playEPKcPN2gf13GF_OBJ_HANDLEEjjjjRKN2mm4Vec3Ef");

		JumpToClosedLandmarks_World::HookAt(&gf::GfMenuObjWorldMap::isEnterMap);
		JumpToClosedLandmarks_Zone::HookAt(&gf::GfMenuObjWorldMap::isOpenLandmark);

		ReplaceTitleEvent::HookAt("_ZN2tl9TitleMain14playTitleEventEj");
#else
		GetWinIDOverride::HookAt("_ZN2ml7DebDraw15getCacheDrawWIDEv");
		RenderViewHook::HookAt("_ZN2ml7WinView10renderViewEv");
#endif
	}

	void DebugStuff::Update() {
		auto& state = GetState();

		bgmTrackIndex = 0;

		if(GetPlayer(2)->InputDownStrict(Keybind::TEST_BUTTON)) {
			// temp space for tests

			//gf::GfPlayFactory::createOpenMenu(state.tempInt, nullptr);

			/*if(state.tempInt < 0)
				fw::Framework::setUpdateSpeed(1.f / abs(state.tempInt));
			else if(state.tempInt > 0)
				fw::Framework::setUpdateSpeed(state.tempInt);*/
		} else if(GetPlayer(2)->InputDownStrict(Keybind::DEBUG_RENDER_TOGGLE)) {
			enableDebugRendering = !enableDebugRendering;
#if !XENOMODS_CODENAME(bfsw)
			fw::PadManager::enableDebugDraw(enableDebugRendering);
#endif
			g_Logger->LogInfo("Debug rendering: {}", enableDebugRendering);
		} else if(GetPlayer(2)->InputDownStrict(Keybind::ACCESS_CLOSED_LANDMARKS)) {
			accessClosedLandmarks = !accessClosedLandmarks;
			g_Logger->LogInfo("Access closed landmarks: {}", accessClosedLandmarks);
		}

		else if(GetPlayer(2)->InputDownStrict(Keybind::TEMPINT_INC)) {
			state.tempInt++;
			g_Logger->LogInfo("TempInt++, now {}", state.tempInt);
		} else if(GetPlayer(2)->InputDownStrict(Keybind::TEMPINT_DEC)) {
			state.tempInt--;
			g_Logger->LogInfo("TempInt--, now {}", state.tempInt);
		} else if(GetPlayer(2)->InputDownStrict(Keybind::MAPJUMP_JUMP)) {
			g_Logger->LogInfo("Attempting jump to MapJump {}", state.tempInt);
			DoMapJump(state.tempInt);
		} else if(GetPlayer(2)->InputDownStrict(Keybind::PLAYSE)) {
			g_Logger->LogInfo("Sound effect {} (0x{:x})", state.tempInt, state.tempInt);
			PlaySE(state.tempInt);
		}

		else if(GetPlayer(2)->InputDownStrict(Keybind::RETURN_TO_TITLE)) {
			PlaySE(gf::GfMenuObjUtil::SEIndex::Sort);
			ReturnTitle(-1);
		}
	}

	XENOMODS_REGISTER_MODULE(DebugStuff);

} // namespace xenomods