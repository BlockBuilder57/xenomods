#include "main.hpp"

#include <skylaunch/hookng/Hooks.hpp>
#include <xenomods/engine/fw/Document.hpp>
#include <xenomods/engine/ml/Filesystem.hpp>
#include <xenomods/engine/ml/Rand.hpp>
#include <xenomods/engine/mm/MathTypes.hpp>

#include "State.hpp"
#include "Version.hpp"
#include "modules/DebugStuff.hpp"
#include "xenomods/DebugWrappers.hpp"
#include "xenomods/HidInput.hpp"
#include "xenomods/Logger.hpp"
#include "xenomods/NnFile.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"

namespace {

#if !XENOMODS_CODENAME(bfsw)
	struct FrameworkUpdateHook : skylaunch::hook::Trampoline<FrameworkUpdateHook> {
		static void Hook(void* framework) {
			Orig(framework);
			xenomods::update();
		}
	};
#else
	fw::Document* document;

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

#if XENOMODS_CODENAME(bf2)
	struct FixIraOnBF2Mount : skylaunch::hook::Trampoline<FixIraOnBF2Mount> {
		static bool Hook(const char* path) {
			auto view = std::string_view(path);
			if(view.starts_with("menu_ira"))
				return false;
			return Orig(path);
		}
	};
#endif

} // namespace

namespace xenomods {

	void fmt_assert_failed(const char* file, int line, const char* message) {
		NN_DIAG_LOG(nn::diag::LogSeverity::Fatal, "fmtlib assert caught @ %s:%d : %s", file, line, message);
	}

	void toastVersion() {
		g_Logger->ToastInfo("xm_version1", "xenomods{} - {}", version::BuildIsDebug() ? " (debug)" : "", version::BuildGitVersion());
		g_Logger->ToastInfo("xm_version2", "built {}", version::BuildTimestamp());
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

		if(P2->InputDownStrict(RELOAD_CONFIG)) {
			GetState().config.LoadFromFile();
			DebugStuff::PlaySE(gf::GfMenuObjUtil::SEIndex::Sort);
		} else if(P2->InputDownStrict(LOGGER_TEST)) {
			g_Logger->LogDebug("test debug message! {}", ml::mtRand());
			g_Logger->LogInfo("test info message! {}", ml::mtRandf1());
			g_Logger->LogWarning("test warning message! {}", ml::mtRandf2());
			g_Logger->LogError("test error message! {}", ml::mtRandf3());
			g_Logger->LogFatal("test fatal message! {}", nn::os::GetSystemTick());

			toastVersion();
			int group = ml::mtRand(100, 999);
			g_Logger->ToastWarning(fmt::format("{}", group), "random group ({})", group);
			g_Logger->ToastMessage("logger test", Logger::Severity::Info, "system tick in seconds: {:2f}", nn::os::GetSystemTick() / 19200000.);
		}

		// Update modules
		xenomods::UpdateAllRegisteredModules();

		// draw log messages
		g_Logger->Draw();
	}

	void main() {
		InitializeAllRegisteredModules();

#if XENOMODS_CODENAME(bf2)
		if(GetState().config.mountTornaContent) {
			NnFile file("rom:/ira-xm.arh", nn::fs::OpenMode_Read);

			if(file) {
				file.Close(); // gotta let the game read it lol
				FixIraOnBF2Mount::HookAt(&ml::DevFileTh::checkValidFileBlock);
				ml::DevFileTh::registArchive(ml::MEDIA::Default, "ira-xm.arh", "ira-xm.ard", "aoc1:/");
			}
		}
#endif

		// hook our updater
#if !XENOMODS_CODENAME(bfsw)
		FrameworkUpdateHook::HookAt("_ZN2fw9Framework6updateEv");
#else
		FrameworkUpdater_updateStdHook::HookAt("_ZN2fw16FrameworkUpdater9updateStdERKNS_8DocumentEPNS_19FrameworkControllerE");
#endif

		toastVersion();
	}

} // namespace xenomods
