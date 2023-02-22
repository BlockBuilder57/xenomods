#include "main.hpp"

#include <xenomods/engine/fw/Document.hpp>
#include <xenomods/engine/ml/Rand.hpp>
#include <xenomods/engine/mm/MathTypes.hpp>
#include <skylaunch/hookng/Hooks.hpp>

#include "State.hpp"
#include "Version.hpp"
#include "xenomods/DebugWrappers.hpp"
#include "xenomods/HidInput.hpp"
#include "xenomods/Logger.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"
#include "modules/DebugStuff.hpp"

namespace fw {


#if !XENOMODS_CODENAME(bfsw)
	struct FrameworkUpdateHook : skylaunch::hook::Trampoline<FrameworkUpdateHook> {
		static void Hook(void* framework) {
			Orig(framework);
			xenomods::update();
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

			xenomods::update();
		}

	};
#endif

} // namespace fw

namespace xenomods {

	void fmt_assert_failed(const char* file, int line, const char* message) {
		NN_DIAG_LOG(nn::diag::LogSeverity::Fatal, "fmtlib assert caught @ %s:%d : %s", file, line, message);
	}

	void update() {
		// lazy
		using enum xenomods::Keybind;

		HidInput* P1 = GetPlayer(1);
		HidInput* P2 = GetPlayer(2);
		P1->Poll();
		P2->Poll();

		/*std::string p1Buttons = fmt::format("{:#08x} - P1 - {:#08x}", P1->stateCur.Buttons, P1->statePrev.Buttons);
		std::string p2Buttons = fmt::format("{:#08x} - P2 - {:#08x}", P2->stateCur.Buttons, P2->statePrev.Buttons);
		int buttonsP1Width = fw::debug::drawFontGetWidth(p1Buttons.c_str());
		int buttonsP2Width = fw::debug::drawFontGetWidth(p2Buttons.c_str());
		fw::debug::drawFontShadow(1280-buttonsP1Width-5, 5, mm::Col4::white, p1Buttons.c_str());
		fw::debug::drawFontShadow(1280-buttonsP2Width-5, 5+16, mm::Col4::white, p2Buttons.c_str());

		auto testcombo = nn::hid::KEY_A;
		if(P2->InputHeld(testcombo))
			fw::debug::drawFontShadow(1280/2, 0, mm::Col4::cyan, "combo held!");
		if (P2->InputDown(testcombo))
			g_Logger->LogDebug("combo down...");
		if (P2->InputUp(testcombo))
			g_Logger->LogDebug("combo up!");
		if(P2->InputHeldStrict(testcombo))
			fw::debug::drawFontShadow(1280/2, 16, mm::Col4::cyan, "strict combo held!");
		if (P2->InputDownStrict(testcombo))
			g_Logger->LogDebug("strict combo down...");
		if (P2->InputUpStrict(testcombo))
			g_Logger->LogDebug("strict combo up!");*/

		/*
		 * Enforce some things on first update
		 */
		static bool hasUpdated;
		if(!hasUpdated) {
			nn::hid::SetSupportedNpadStyleSet(3);
#if !XENOMODS_CODENAME(bfsw)
			fw::PadManager::enableDebugDraw(true);
#endif
			hasUpdated = true;
		}

		/*
		 * Check buttons
		 */

		if (P2->InputDownStrict(RELOAD_CONFIG)) {
			GetState().config.LoadFromFile();
			DebugStuff::PlaySE(gf::GfMenuObjUtil::SEIndex::Sort);
		} else if(P2->InputDownStrict(LOGGER_TEST)) {
			g_Logger->LogDebug("test debug message! {}", ml::mtRand());
			g_Logger->LogInfo("test info message! {}", ml::mtRandf1());
			g_Logger->LogWarning("test warning message! {}", ml::mtRandf2());
			g_Logger->LogError("test error message! {}", ml::mtRandf3());
			g_Logger->LogFatal("test fatal message! {}", nn::os::GetSystemTick());
		}

		// Update modules
		xenomods::UpdateAllRegisteredModules();

		// draw log messages
		g_Logger->Draw();
	}

	void main() {
		InitializeAllRegisteredModules();

		// hook our updater
#if !XENOMODS_CODENAME(bfsw)
		fw::FrameworkUpdateHook::HookAt("_ZN2fw9Framework6updateEv");
#else
		fw::FrameworkUpdater_updateStdHook::HookAt("_ZN2fw16FrameworkUpdater9updateStdERKNS_8DocumentEPNS_19FrameworkControllerE");
#endif

		g_Logger->LogInfo("xenomods{} - {} initialized (built {})", version::IsDebug() ? " (debug)" : "", version::GitVersion(), version::BuildTimestamp());
	}

} // namespace xenomods
