#include "main.hpp"

#include <bf2mods/engine/fw/Document.hpp>
#include <bf2mods/engine/ml/Rand.hpp>
#include <bf2mods/engine/mm/MathTypes.hpp>
#include <skylaunch/hookng/Hooks.hpp>

#include "State.hpp"
#include "bf2mods/DebugWrappers.hpp"
#include "bf2mods/HidInput.hpp"
#include "bf2mods/Logger.hpp"
#include "bf2mods/stuff/utils/debug_util.hpp"
#include "modules/DebugStuff.hpp"
#include "version.h"

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

	void update() {
		// lazy
		using enum bf2mods::Keybind;

		HidInput* P1 = GetPlayer(1);
		HidInput* P2 = GetPlayer(2);
		P1->Poll();
		P2->Poll();

		/*std::string p1Buttons = fmt::format("{:#08x} - P1 - {:#08x}", P1->stateCur.Buttons, P1->statePrev.Buttons);
		std::string p2Buttons = fmt::format("{:#08x} - P2 - {:#08x}", P2->stateCur.Buttons, P2->statePrev.Buttons);
		int buttonsP1Width = fw::debug::drawFontGetWidth(p1Buttons.c_str());
		int buttonsP2Width = fw::debug::drawFontGetWidth(p2Buttons.c_str());
		fw::debug::drawFontShadow(1280-buttonsP1Width-5, 5, mm::Col4::white, p1Buttons.c_str());
		fw::debug::drawFontShadow(1280-buttonsP2Width-5, 5+16, mm::Col4::white, p2Buttons.c_str());*/

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

		if(P2->InputDownStrict(CLEAR_TCPLOG)) {
			// https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#text-modification
			skylaunch::logger::s_Instance->LogFormat("\u001B[2J");

			g_Logger->LogInfo("Cleared TCP log");
			DebugStuff::PlaySE(gf::GfMenuObjUtil::SEIndex::Sort);
		} else if(P2->InputDownStrict(LOGGER_TEST)) {
			g_Logger->LogDebug("test debug message! {}", ml::mtRand());
			g_Logger->LogInfo("test info message! {}", ml::mtRandf1());
			g_Logger->LogWarning("test warning message! {}", ml::mtRandf2());
			g_Logger->LogError("test error message! {}", ml::mtRandf3());
			g_Logger->LogFatal("test fatal message! {}", nn::os::GetSystemTick());
		}

		// Update modules
		bf2mods::UpdateAllRegisteredModules();

		// draw log messages
		g_Logger->Draw();
	}

	void main() {
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
