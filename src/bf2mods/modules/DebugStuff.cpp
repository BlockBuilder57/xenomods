#include "DebugStuff.hpp"

#include <map>
#include <skylaunch/hookng/Hooks.hpp>

#include "../State.hpp"
#include "../main.hpp"
#include "bf2mods/Logger.hpp"
#include "bf2mods/Utils.hpp"
#include "bf2mods/engine/bdat/Bdat.hpp"
#include "bf2mods/engine/fw/Debug.hpp"
#include "bf2mods/engine/fw/Framework.hpp"
#include "bf2mods/engine/game/MapJump.hpp"
#include "bf2mods/engine/gf/BdatData.hpp"
#include "bf2mods/engine/gf/PlayFactory.hpp"
#include "bf2mods/engine/mm/MathTypes.hpp"
#include "bf2mods/engine/tl/title.hpp"
#include "bf2mods/stuff/utils/util.hpp"

namespace {

	struct MMAssert : skylaunch::hook::Trampoline<MMAssert> {
		static void Hook(const char* expr, const char* file, unsigned line) {
			bf2mods::g_Logger->LogFatal("Caught Assert!!! Expr \"{}\" ({} : {})", expr, file, line);
			Orig(expr, file, line);
		}
	};

	struct EventStartInfo : skylaunch::hook::Trampoline<EventStartInfo> {
		static void Hook(void* this_pointer, const char* evtName, void* objHandle, uint param_3, uint param_4, uint param_5, uint param_6, mm::Vec3* playerPos, float param_8) {
			std::string eventName = evtName;
			if(eventName.size() > 10)
				eventName.resize(10);
			uint eventId = gf::GfDataEvent::getEventID(eventName.c_str());

			if(eventId > 0)
				bf2mods::g_Logger->LogDebug("Creating event {} (id {})", evtName, eventId);
			else
				bf2mods::g_Logger->LogDebug("Creating event {}", evtName);
			//bf2mods::g_Logger->LogDebug("Other args: {:p} {} {} {} {} {:2} {:2}", objHandle, param_3, param_4, param_5, param_6, static_cast<const glm::vec3&>(*playerPos), param_8);

			Orig(this_pointer, evtName, objHandle, param_3, param_4, param_5, param_6, playerPos, param_8);
		}
	};

	struct ReplaceTitleEvent : skylaunch::hook::Trampoline<ReplaceTitleEvent> {
		static void Hook(tl::TitleMain* this_pointer, uint eventId) {
			uint newEventId = eventId;

			// get the clear count from the save because that's what everything else seems to do
			uint clearCount = *reinterpret_cast<uint*>(reinterpret_cast<char*>(this_pointer->getSaveBuffer()) + 0x109b3c);
			uint chapter = this_pointer->getChapterIdFromSaveData();
			chapter |= clearCount << 16; // "encodes" as 0x00010006 for 1 clear on ch6

			// we need to have started the game (to avoid a lock)
			if(chapter > 0) {
				auto& events = bf2mods::GetState().config.titleEvents;

				if(!events.empty()) {
					const auto eventsDefault = std::vector<uint16_t>(CONFIG_TITLEEVENTS_DEFAULT);
					if (events == eventsDefault && chapter <= 10) {
						// we're still the default, so let's be fancy and progressively reveal chapters
						// once the user clears the game at least once this path will never be run,
						// so the extra stuff in the default (post-credits titlescreens) won't be shown
						newEventId = events[(util::nnRand<uint16_t>() % (chapter + 1))];
					}
					else
						newEventId = events[(util::nnRand<uint16_t>() % (events.size() + 1))];
				}
			}

			if(newEventId != eventId)
				bf2mods::g_Logger->LogDebug("Replacing title event {} (id {}) with {} (id {})", gf::GfDataEvent::getEventName(eventId), eventId, gf::GfDataEvent::getEventName(newEventId), newEventId);
			Orig(this_pointer, newEventId);
		}
	};

}

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

		MMAssert::HookAt("_ZN2mm9MMStdBase8mmAssertEPKcS2_j");

#if !BF2MODS_CODENAME(bfsw)
		// TODO: get the bgm debugging stuff that's already present to work

		EventStartInfo::HookAt("_ZN5event7Manager4playEPKcPN2gf13GF_OBJ_HANDLEEjjjjRKN2mm4Vec3Ef");
		ReplaceTitleEvent::HookAt("_ZN2tl9TitleMain14playTitleEventEj");
#endif
	}

	void DebugStuff::Update() {
		auto& state = GetState();

		bgmTrackIndex = 0;

		if(btnDown(Keybind::TEST_BUTTON, p2Cur.Buttons, p2Prev.Buttons)) {
			// temp space for tests

			if(state.tempInt < 0)
				fw::Framework::setUpdateSpeed(1.f / abs(state.tempInt));
			else if(state.tempInt > 0)
				fw::Framework::setUpdateSpeed(state.tempInt);
		} else if(btnDown(Keybind::DEBUG_RENDER_TOGGLE, p2Cur.Buttons, p2Prev.Buttons)) {
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