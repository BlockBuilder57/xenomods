//
// Created by block on 1/18/2023.
//

#include "EventDebugUtils.hpp"

#include "../main.hpp"
#include "DebugStuff.hpp"
#include "bf2mods/stuff/utils/debug_util.hpp"
#include "bf2mods/DebugWrappers.hpp"
#include "bf2mods/Logger.hpp"
#include "bf2mods/engine/event/Manager.hpp"

#include <skylaunch/hookng/Hooks.hpp>

namespace {

	struct DrawEventManagerInfo : skylaunch::hook::Trampoline<DrawEventManagerInfo> {
		static void Hook(event::Manager* this_pointer) {
			bf2mods::EventDebugUtils::ShouldUpdate = !this_pointer->isPlayCancel();

			if(!this_pointer->isPlayCancel() && bf2mods::DebugStuff::enableDebugRendering) {
				if ((bf2mods::EventDebugUtils::ActiveBits >> registeredIndex) & 1) {
					this_pointer->drawInfo();
				}
			}

			return Orig(this_pointer);
		}

		static void HookIt(bool enable = false) {
			HookAt(&event::Manager::update);
			registeredIndex = bf2mods::EventDebugUtils::RegistrationIndex++;
			auto name = dbgutil::getSymbol(std::bit_cast<uintptr_t>(skylaunch::FlattenMemberPtr(&event::Manager::update)));
			name = name.substr(0, name.find("::", name.find_first_of("::")+2));
			bf2mods::EventDebugUtils::FuncNames.emplace_back(name);

			if (enable)
				bf2mods::EventDebugUtils::ActiveBits |= 1 << registeredIndex;
		}

		static int registeredIndex;
	};

	template<class TManager>
	struct ManagerDisplay : skylaunch::hook::Trampoline<ManagerDisplay<TManager>> {
		using HookType = skylaunch::hook::Trampoline<ManagerDisplay<TManager>>;

		static void Hook(TManager* this_pointer, event::MSG_TYPE msg) {
			HookType::Orig(this_pointer, msg);
			if (msg == 0xe && bf2mods::DebugStuff::enableDebugRendering && (bf2mods::EventDebugUtils::ActiveBits >> registeredIndex) & 1) {
				this_pointer->setDisp(true);
				this_pointer->render();
			}
		}

		static void HookIt(bool enable = false) {
			HookType::HookAt(&TManager::OnEventProc);
			registeredIndex = bf2mods::EventDebugUtils::RegistrationIndex++;
			auto name = dbgutil::getSymbol(std::bit_cast<uintptr_t>(skylaunch::FlattenMemberPtr(&TManager::OnEventProc)));
			name = name.substr(0, name.find("::", name.find_first_of("::")+2)); // lobs off anything past event::ManagerName
			bf2mods::EventDebugUtils::FuncNames.emplace_back(name);

			if (enable)
				bf2mods::EventDebugUtils::ActiveBits |= 1 << registeredIndex;
		}

		static int registeredIndex;
	};

	template<class TManager>
	int ManagerDisplay<TManager>::registeredIndex = 0;

	int DrawEventManagerInfo::registeredIndex = 0;

}

namespace bf2mods {

	int EventDebugUtils::RegistrationIndex = 0;
	int EventDebugUtils::CurrentIndex = -1;
	unsigned long EventDebugUtils::ActiveBits = 0;
	std::vector<std::string> EventDebugUtils::FuncNames{};

	bool EventDebugUtils::ShouldUpdate = false;

	void EventDebugUtils::Update() {
		if (FuncNames.size() != RegistrationIndex) {
			bf2mods::g_Logger->LogError("Event function names not the same size as regist index {} vs {}", FuncNames.size(), RegistrationIndex);
			return;
		}

		bool changed = false;

		//fw::debug::drawFontFmtShadow(0, 720-16, mm::Col4::White, "Manager bits: {:0b}", ActiveBits);

		if (btnDown(Keybind::EVENT_DEBUG_PREV, p1Cur.Buttons, p1Prev.Buttons)) {
			CurrentIndex--;
			changed = true;
		}
		else if (btnDown(Keybind::EVENT_DEBUG_NEXT, p1Cur.Buttons, p1Prev.Buttons)) {
			CurrentIndex++;
			changed = true;
		}

		if (changed) {
			if (CurrentIndex < 0)
				CurrentIndex = RegistrationIndex -1;
			else if (CurrentIndex >= RegistrationIndex)
				CurrentIndex = 0;

			g_Logger->LogInfo("Selected {} (currently {})", FuncNames[CurrentIndex], (ActiveBits >> CurrentIndex) & 1 ? "on" : "off");
		}

		if (btnDown(Keybind::EVENT_DEBUG_TOGGLE, p1Cur.Buttons, p1Prev.Buttons)) {
			if (CurrentIndex > 0 && CurrentIndex < RegistrationIndex) {
				ActiveBits ^= 1 << CurrentIndex;
				g_Logger->LogInfo("Toggled debug for {} (now {})", FuncNames[CurrentIndex], (ActiveBits >> CurrentIndex) & 1 ? "on" : "off");
			}
			else
				g_Logger->LogWarning("Tried to toggle invalid debug manager, try another (was index {})", CurrentIndex);
		}
	}

	void EventDebugUtils::Initialize() {
		g_Logger->LogDebug("Setting up event debug utils...");

		DrawEventManagerInfo::HookIt(true);

		ManagerDisplay<event::AgelogManager>::HookIt();
		ManagerDisplay<event::AlphaManager>::HookIt();
		ManagerDisplay<event::BgmManager>::HookIt(true);
		ManagerDisplay<event::BouManager>::HookIt();
		ManagerDisplay<event::CamManager>::HookIt(true);
		ManagerDisplay<event::CsSndManager>::HookIt(true);
		ManagerDisplay<event::DebugManager>::HookIt(true);
		ManagerDisplay<event::DofManager>::HookIt();
		ManagerDisplay<event::EnvManager>::HookIt();
		ManagerDisplay<event::EvtxtManager>::HookIt();
		ManagerDisplay<event::FrameManager>::HookIt(true);
		ManagerDisplay<event::GroupManager>::HookIt();
		ManagerDisplay<event::LightManager>::HookIt();
		ManagerDisplay<event::MemoryManager>::HookIt();
		ManagerDisplay<event::MovieManager>::HookIt(true);
		ManagerDisplay<event::ObjectManager>::HookIt();
		ManagerDisplay<event::OutsiderManager>::HookIt();
		ManagerDisplay<event::ResManager>::HookIt();
		ManagerDisplay<event::SeamlessManager>::HookIt();
		ManagerDisplay<event::SeManager>::HookIt(true);
		ManagerDisplay<event::SeqManager>::HookIt(true);
		ManagerDisplay<event::TextManager>::HookIt(true);
		ManagerDisplay<event::TodoManager>::HookIt();
		ManagerDisplay<event::VoiceManager>::HookIt(true);
		ManagerDisplay<event::VolumeManager>::HookIt(true);
	}

#if BF2MODS_CODENAME(bf2) || BF2MODS_CODENAME(ira)
	BF2MODS_REGISTER_MODULE(EventDebugUtils);
#endif
}