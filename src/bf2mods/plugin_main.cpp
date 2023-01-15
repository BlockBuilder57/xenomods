#include "plugin_main.hpp"

#include <bf2mods/engine/fw/document.hpp>
#include <bf2mods/engine/mm/math_types.hpp>

#include "bf2mods/bf2logger.hpp"
#include "bf2mods/debug_wrappers.hpp"
#include "bf2mods/stuff/utils/debug_util.hpp"
#include "state.hpp"
#include "version.h"

#include <skylaunch/hookng/Hooks.hpp>

// all parts here
#include "bdat_randomizer.hpp"
#include "camera_tools.hpp"
#include "debug_stuff.hpp"
#include "menu_viewer.hpp"
#include "player_movement.hpp"

namespace fw {


#if !BF2MODS_CODENAME(bfsw)
	struct FrameworkUpdateHook : skylaunch::hook::Trampoline<FrameworkUpdateHook> {
		static void Hook(void* framework) {
			Orig(framework);
			bf2mods::update();
		}
	};
#else
	Document* document;

	struct FrameworkUpdater_updateStdHook : skylaunch::hook::Trampoline<FrameworkUpdater_updateStdHook> {
		static void Hook(fw::Document* doc, void* FrameworkController) {
			Orig(doc, FrameworkController);

			if(doc != nullptr && document == nullptr) {
				document = doc;
			}

			bf2mods::update();
		}

	};
#endif

} // namespace fw

namespace bf2mods {

	void fmt_assert_failed(const char* file, int line, const char* message) {
		NN_DIAG_LOG(nn::diag::LogSeverity::Fatal, "fmtlib assert caught @ %s:%d : %s", file, line, message);
	}

	HidInput p1Cur {};
	HidInput p1Prev {};
	HidInput p2Cur {};
	HidInput p2Prev {};

	void update() {
		// lazy
		using enum bf2mods::Keybind;

		// Read controllers
		p1Cur.Buttons = 0ul;
		p2Cur.Buttons = 0ul;

		nn::hid::NpadHandheldState npadHandheldState {};
		nn::hid::NpadFullKeyState npadP1FullKeyState {};
		nn::hid::NpadFullKeyState npadP2FullKeyState {};

		nn::hid::GetNpadState(&npadHandheldState, nn::hid::CONTROLLER_HANDHELD);
		nn::hid::GetNpadState(&npadP1FullKeyState, nn::hid::CONTROLLER_PLAYER_1);
		nn::hid::GetNpadState(&npadP2FullKeyState, nn::hid::CONTROLLER_PLAYER_2);

		auto updateHidInput = [&](nn::hid::NpadHandheldState& state, bf2mods::HidInput& cur) {
			if(state.Flags & nn::hid::NpadFlags::NPAD_CONNECTED) {
				cur.Buttons |= state.Buttons;
				cur.LAxis = glm::vec2(static_cast<float>(state.LStickX) / 32768.f, static_cast<float>(state.LStickY) / 32768.f);
				cur.RAxis = glm::vec2(static_cast<float>(state.RStickX) / 32768.f, static_cast<float>(state.RStickY) / 32768.f);
			}
		};

		updateHidInput(npadHandheldState, p1Cur);
		updateHidInput(npadP1FullKeyState, p1Cur);
		updateHidInput(npadP2FullKeyState, p2Cur);

		//int buttonsP1Width = fw::debug::drawFontGetWidth("%xh - P1", p1Cur.Buttons);
		//int buttonsP2Width = fw::debug::drawFontGetWidth("%xh - P2", p2Cur.Buttons);
		//fw::debug::drawFont(1280-buttonsP1Width-5, 5, mm::Col4::White, "%xh - P1", p1Cur.Buttons);
		//fw::debug::drawFont(1280-buttonsP2Width-5, 5+16, mm::Col4::White, "%xh - P2", p2Cur.Buttons);

		/*
		 * Enforce some things on first update
		 */
		static bool hasUpdated;
		if(!hasUpdated) {
			nn::hid::SetSupportedNpadStyleSet(3);
#if !BF2MODS_CODENAME(bfsw)
			fw::PadManager::enableDebugDraw(true);
#endif
			hasUpdated = true;
		}

		/*
		 * Check buttons
		 */

		auto& state = GetState();

		if(btnDown(CLEAR_TCPLOG, p2Cur.Buttons, p2Prev.Buttons)) {
			// https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#text-modification
			skylaunch::logger::s_Instance->LogFormat("\u001B[2J");

			g_Logger->LogInfo("Cleared TCP log");
			DebugStuff::PlaySE(gf::GfMenuObjUtil::SEIndex::Sort);
		} else if(btnDown(LOGGER_TEST, p2Cur.Buttons, p2Prev.Buttons)) {
			g_Logger->LogDebug("test debug message! {}", nn::os::GetSystemTick());
			g_Logger->LogInfo("test info message! {}", nn::os::GetSystemTick());
			g_Logger->LogWarning("test warning message! {}", nn::os::GetSystemTick());
			g_Logger->LogError("test error message! {}", nn::os::GetSystemTick());
			g_Logger->LogFatal("test fatal message! {}", nn::os::GetSystemTick());
		}

		// Update modules
		bf2mods::UpdateAllRegisteredModules();

		p1Prev = p1Cur;
		p2Prev = p2Cur;

		// draw log messages
		g_Logger->Draw();
	}

	void main() {

		// mount sd
		Result rc = nn::fs::MountSdCardForDebug("sd");
		g_Logger->LogDebug("Mounted SD card (result 0x{:x})", rc);

		InitializeAllRegisteredModules();

		// hook our updater
#if !BF2MODS_CODENAME(bfsw)
		fw::FrameworkUpdateHook::HookAt("_ZN2fw9Framework6updateEv");
#else
		fw::FrameworkUpdater_updateStdHook::HookAt("_ZN2fw16FrameworkUpdater9updateStdERKNS_8DocumentEPNS_19FrameworkControllerE");
#endif

#if _DEBUG
		g_Logger->LogInfo("bf2mods (debug) - {} initialized (built {} {})", version::tagDirty, __DATE__, __TIME__);
#else
		g_Logger->LogInfo("bf2mods - {} initalized", version::tagDirty);
#endif
	}

} // namespace bf2mods
