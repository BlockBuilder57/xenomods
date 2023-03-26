// Created by block on 2/26/2023.

#include "DEDebugTests.hpp"
#include "DebugStuff.hpp"

#include <xenomods/DebugWrappers.hpp>
#include <xenomods/HidInput.hpp>
#include <xenomods/Logger.hpp>
#include <xenomods/Utils.hpp>
#include <skylaunch/hookng/Hooks.hpp>

#include "../State.hpp"
#include "../main.hpp"
#include "xenomods/engine/game/DebugDraw.hpp"
#include "xenomods/engine/ml/WinView.hpp"

#if XENOMODS_CODENAME(bfsw)
namespace {

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

}

namespace xenomods {

	void DEDebugTests::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up DE debug tests...");

		game::DebugDraw::setEnable(static_cast<game::DebugDraw::Group>(-1), true);
		game::DebugDraw::drawFont(static_cast<game::DebugDraw::Group>(-1), 0, 0, mm::Col4::white, "please work", 1.f);

		GetWinIDOverride::HookAt("_ZN2ml7DebDraw15getCacheDrawWIDEv");
		RenderViewHook::HookAt("_ZN2ml7WinView10renderViewEv");
	}

	void DEDebugTests::Update(fw::UpdateInfo* updateInfo) {

	}

	//XENOMODS_REGISTER_MODULE(DEDebugTests);

} // namespace xenomods
#endif
