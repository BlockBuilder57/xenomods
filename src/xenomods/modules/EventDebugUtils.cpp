//
// Created by block on 1/18/2023.
//

#include "EventDebugUtils.hpp"

#include <skylaunch/hookng/Hooks.hpp>
#include <xenomods/DebugWrappers.hpp>
#include <xenomods/HidInput.hpp>
#include <xenomods/Logger.hpp>
#include <xenomods/Utils.hpp>

#include "../State.hpp"
#include "DebugStuff.hpp"
#include "xenomods/engine/event/Manager.hpp"
#include "xenomods/engine/gf/BdatData.hpp"
#include "xenomods/engine/ml/Rand.hpp"
#include "xenomods/engine/tl/title.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"

namespace {

	struct DrawEventManagerInfo : skylaunch::hook::Trampoline<DrawEventManagerInfo> {
		static void Hook(event::Manager* this_pointer) {
			xenomods::EventDebugUtils::ShouldUpdate = !this_pointer->isPlayCancel();

			if(!this_pointer->isPlayCancel() && xenomods::DebugStuff::enableDebugRendering) {
				if((xenomods::EventDebugUtils::ActiveBits >> registeredIndex) & 1) {
					this_pointer->drawInfo();
				}
			}

			return Orig(this_pointer);
		}

		static void HookIt() {
			HookAt(&event::Manager::update);
			registeredIndex = xenomods::EventDebugUtils::RegistrationIndex++;
			auto name = dbgutil::getSymbol(std::bit_cast<uintptr_t>(skylaunch::FlattenMemberPtr(&event::Manager::update)));
			name = name.substr(0, name.find("::", name.find_first_of("::") + 2));
			xenomods::EventDebugUtils::FuncNames.emplace_back(name);
		}

		static int registeredIndex;
	};

	template<class TManager>
	struct ManagerDisplay : skylaunch::hook::Trampoline<ManagerDisplay<TManager>> {
		using HookType = skylaunch::hook::Trampoline<ManagerDisplay<TManager>>;

		static void Hook(TManager* this_pointer, event::MSG_TYPE msg) {
			HookType::Orig(this_pointer, msg);
			if(msg == 0xe && xenomods::DebugStuff::enableDebugRendering && (xenomods::EventDebugUtils::ActiveBits >> registeredIndex) & 1) {
				this_pointer->setDisp(true);
				this_pointer->render();
			}
		}

		static void HookIt() {
			HookType::HookAt(&TManager::OnEventProc);
			registeredIndex = xenomods::EventDebugUtils::RegistrationIndex++;
			auto name = dbgutil::getSymbol(std::bit_cast<uintptr_t>(skylaunch::FlattenMemberPtr(&TManager::OnEventProc)));
			name = name.substr(0, name.find("::", name.find_first_of("::") + 2)); // lobs off anything past event::ManagerName
			xenomods::EventDebugUtils::FuncNames.emplace_back(name);
		}

		static int registeredIndex;
	};

	template<class TManager>
	int ManagerDisplay<TManager>::registeredIndex = 0;

	int DrawEventManagerInfo::registeredIndex = 0;


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

} // namespace

namespace xenomods {

	int EventDebugUtils::RegistrationIndex = 0;
	int EventDebugUtils::CurrentIndex = -1;
	unsigned long EventDebugUtils::ActiveBits = 0;
	std::vector<std::string> EventDebugUtils::FuncNames {};

	bool EventDebugUtils::ShouldUpdate = false;

	void EventDebugUtils::Update(fw::UpdateInfo* updateInfo) {
		if(FuncNames.size() != RegistrationIndex) {
			xenomods::g_Logger->LogError("Event function names not the same size as regist index {} vs {}", FuncNames.size(), RegistrationIndex);
			return;
		}

		bool changed = false;

		//fw::debug::drawFontFmtShadow(0, 720-16, mm::Col4::white, "Manager bits: {:0b}", ActiveBits);

		if(GetPlayer(1)->InputDownStrict(Keybind::EVENT_DEBUG_PREV)) {
			CurrentIndex--;
			changed = true;
		} else if(GetPlayer(1)->InputDownStrict(Keybind::EVENT_DEBUG_NEXT)) {
			CurrentIndex++;
			changed = true;
		}

		if(changed) {
			if(CurrentIndex < 0)
				CurrentIndex = RegistrationIndex - 1;
			else if(CurrentIndex >= RegistrationIndex)
				CurrentIndex = 0;

			g_Logger->LogInfo("Selected {} (currently {})", FuncNames[CurrentIndex], (ActiveBits >> CurrentIndex) & 1 ? "on" : "off");
		}

		if(GetPlayer(1)->InputDownStrict(Keybind::EVENT_DEBUG_TOGGLE)) {
			if(CurrentIndex >= 0 && CurrentIndex < RegistrationIndex) {
				ActiveBits ^= 1 << CurrentIndex;
				g_Logger->LogInfo("Toggled debug for {} (now {})", FuncNames[CurrentIndex], (ActiveBits >> CurrentIndex) & 1 ? "on" : "off");
			} else
				g_Logger->LogWarning("Tried to toggle invalid debug manager, try another (was index {})", CurrentIndex);
		}
	}

	void EventDebugUtils::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up event debug utils...");

		ActiveBits = xenomods::GetState().config.eventDebugBits;

		DrawEventManagerInfo::HookIt();                 // bit 0
		ManagerDisplay<event::AgelogManager>::HookIt(); // bit 1
		ManagerDisplay<event::AlphaManager>::HookIt();	// bit ...
		ManagerDisplay<event::BgmManager>::HookIt();
		ManagerDisplay<event::BouManager>::HookIt();
		ManagerDisplay<event::CamManager>::HookIt();
		ManagerDisplay<event::CsSndManager>::HookIt();
		ManagerDisplay<event::DebugManager>::HookIt();
		ManagerDisplay<event::DofManager>::HookIt();
		ManagerDisplay<event::EnvManager>::HookIt();
		ManagerDisplay<event::EvtxtManager>::HookIt();
		ManagerDisplay<event::FrameManager>::HookIt();
		ManagerDisplay<event::GroupManager>::HookIt();
		ManagerDisplay<event::LightManager>::HookIt();
		ManagerDisplay<event::MemoryManager>::HookIt();
		ManagerDisplay<event::MovieManager>::HookIt();
		ManagerDisplay<event::ObjectManager>::HookIt();
		ManagerDisplay<event::OutsiderManager>::HookIt();
		ManagerDisplay<event::ResManager>::HookIt();
		ManagerDisplay<event::SeamlessManager>::HookIt();
		ManagerDisplay<event::SeManager>::HookIt();
		ManagerDisplay<event::SeqManager>::HookIt();
		ManagerDisplay<event::TextManager>::HookIt();
		ManagerDisplay<event::TodoManager>::HookIt();
		ManagerDisplay<event::VoiceManager>::HookIt();
		ManagerDisplay<event::VolumeManager>::HookIt();

		// earlier versions didn't include the last two parameters
		if (skylaunch::hook::detail::ResolveSymbolBase("_ZN5event7Manager4playEPKcPN2gf13GF_OBJ_HANDLEEjjjjRKN2mm4Vec3Ef") == 0xDEADDEAD)
			EventStartInfo_Earlier::HookAt("_ZN5event7Manager4playEPKcPN2gf13GF_OBJ_HANDLEEjjjj");
		else
			EventStartInfo::HookAt("_ZN5event7Manager4playEPKcPN2gf13GF_OBJ_HANDLEEjjjjRKN2mm4Vec3Ef");

		ReplaceTitleEvent::HookAt(&tl::TitleMain::playTitleEvent);
	}

#if XENOMODS_CODENAME(bf2) || XENOMODS_CODENAME(ira)
	XENOMODS_REGISTER_MODULE(EventDebugUtils);
#endif
} // namespace xenomods