//
// Created by block on 1/18/2023.
//

#include "EventDebugUtils.hpp"

#include <xenomods/DebugWrappers.hpp>
#include <xenomods/HidInput.hpp>
#include <xenomods/Logger.hpp>
#include <xenomods/Utils.hpp>
#include <skylaunch/hookng/Hooks.hpp>

#include "../State.hpp"
#include "DebugStuff.hpp"
#include "xenomods/engine/event/Manager.hpp"
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

} // namespace

namespace xenomods {

	int EventDebugUtils::RegistrationIndex = 0;
	int EventDebugUtils::CurrentIndex = -1;
	unsigned long EventDebugUtils::ActiveBits = 0;
	std::vector<std::string> EventDebugUtils::FuncNames {};

	bool EventDebugUtils::ShouldUpdate = false;

	void EventDebugUtils::Update() {
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
	}

#if XENOMODS_CODENAME(bf2) || XENOMODS_CODENAME(ira)
	XENOMODS_REGISTER_MODULE(EventDebugUtils);
#endif
} // namespace xenomods