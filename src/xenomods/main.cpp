#include "main.hpp"

#include <skylaunch/hookng/Hooks.hpp>
#include <xenomods/DebugWrappers.hpp>
#include <xenomods/HidInput.hpp>
#include <xenomods/Logger.hpp>
#include <xenomods/NnFile.hpp>
#include <xenomods/Version.hpp>

#include "State.hpp"
#include "modules/DebugStuff.hpp"
#include "xenomods/engine/fw/Document.hpp"
#include "xenomods/engine/fw/Framework.hpp"
#include "xenomods/engine/fw/Managers.hpp"
#include "xenomods/engine/ml/DevGraph.hpp"
#include "xenomods/engine/ml/Filesystem.hpp"
#include "xenomods/engine/ml/Rand.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"

namespace {

#if XENOMODS_OLD_ENGINE
	struct FrameworkUpdateHook : skylaunch::hook::Trampoline<FrameworkUpdateHook> {
		static void Hook(fw::Framework* framework) {
			Orig(framework);

			if (framework != nullptr)
				xenomods::update(framework->getUpdateInfo());
		}
	};
#else
	struct FrameworkUpdater_updateStdHook : skylaunch::hook::Trampoline<FrameworkUpdater_updateStdHook> {
		static void Hook(fw::Document* doc, void* FrameworkController) {
			Orig(doc, FrameworkController);

			if(doc != nullptr && doc->applet != nullptr) {
				xenomods::DocumentPtr = doc;
				xenomods::update(&doc->applet->updateInfo);
			}
		}
	};
#endif

	// Game-specific

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

#if XENOMODS_CODENAME(bfsw)
	struct EnableDebugDrawing : skylaunch::hook::Trampoline<EnableDebugDrawing> {
		static void Hook(ml::Scn* this_pointer, ml::SCNCB callback) {
			Orig(this_pointer, callback);

			ml::DevGraph::cmdOpenDisplayList(0);
			ml::DebDraw::flushPrio(-2, mm::Mat44::identity, mm::Mat44::identity);
			ml::DevGraph::cmdCloseDisplayList();
		}
	};
#elif XENOMODS_CODENAME(bf3)
	struct EnableDebugDrawing : skylaunch::hook::Trampoline<EnableDebugDrawing> {
		static void Hook(ml::Scn* this_pointer, ml::SCNCB callback) {
			Orig(this_pointer, callback);

			mm::Mat44 identity = {};
			((void(*)(unsigned int))(skylaunch::utils::g_MainTextAddr + 0x1248774))(0);
			((void(*)(int, const mm::Mat44&, const mm::Mat44&))(skylaunch::utils::g_MainTextAddr + 0x124bdec))(-1, identity, identity);
			((void(*)())(skylaunch::utils::g_MainTextAddr + 0x1248798))();
		}
	};
#endif

	// Multi-controller stuff

	// Can accept handheld, dual joycons, and pro controller
	struct NpadStyleSetOverride : skylaunch::hook::Trampoline<NpadStyleSetOverride> {
		static void Hook(int) {
			return Orig(7);
		}
	};

	struct DisableControllerSingleMode : skylaunch::hook::Trampoline<DisableControllerSingleMode> {
		static void Hook(nn::hid::ControllerSupportResultInfo* resultInfo, nn::hid::ControllerSupportArg* supportArg) {
			nn::hid::NpadFullKeyState p1State {};
			nn::hid::GetNpadState(&p1State, nn::hid::CONTROLLER_PLAYER_1);

			if(p1State.Flags & nn::hid::NpadFlags::NPAD_CONNECTED) {
				// We have a player 1 at this point, so disable single-controller only mode
				// That way if another controller is connected while the game is running, we can allow it to be P2
				// Monolib actually passes the (according to switchbrew) default arguments to this,
				// so we technically get 4 max controllers for free!
				supportArg->mSingleMode = false;
			}

			Orig(resultInfo, supportArg);
		}
	};

	// Monolib gets very upset in 3 when the controller count is above 1, so let's clamp that down
	struct ClampNumberOfControllers : skylaunch::hook::Trampoline<ClampNumberOfControllers> {
		static int Hook() {
			int result = Orig();

			if (result > 1)
				return 1;

			return result;
		}
	};

} // namespace

namespace xenomods {

#if XENOMODS_NEW_ENGINE
	fw::Document* DocumentPtr = {};
#endif

void fmt_assert_failed(const char* file, int line, const char* message) {
		NN_DIAG_LOG(nn::diag::LogSeverity::Fatal, "fmtlib assert caught @ %s:%d : %s", file, line, message);
	}

	void toastVersion() {
		g_Logger->ToastInfo("xm_version1", "xenomods {}{} [{}]", version::BuildGitVersion(), version::BuildIsDebug ? " (debug)" : "", XENOMODS_CODENAME_STR);
		g_Logger->ToastDebug("xm_version2", "compiled on {}", version::BuildTimestamp());
		g_Logger->ToastDebug("xm_version3", "running {}, version {}", version::RuntimeGame(), version::RuntimeVersion());
		g_Logger->ToastDebug("xm_version4", "exefs {}", version::RuntimeBuildRevision());
	}

	void update(fw::UpdateInfo* updateInfo) {
		// lazy
		using enum xenomods::Keybind;

		HidInput* P1 = GetPlayer(1);
		HidInput* P2 = GetPlayer(2);
		P1->Poll();
		P2->Poll();

		/*std::string p1Buttons = fmt::format("{:#08x} - P1 - {:#08x}", P1->stateCur.Buttons, P1->statePrev.Buttons);
		std::string p2Buttons = fmt::format("{:#08x} - P2 - {:#08x}", P2->stateCur.Buttons, P2->statePrev.Buttons);
		int buttonsP1Width = xenomods::debug::drawFontGetWidth(p1Buttons.c_str());
		int buttonsP2Width = xenomods::debug::drawFontGetWidth(p2Buttons.c_str());
		xenomods::debug::drawFontShadow(1280-buttonsP1Width-5, 5, mm::Col4::white, p1Buttons.c_str());
		xenomods::debug::drawFontShadow(1280-buttonsP2Width-5, 5+16, mm::Col4::white, p2Buttons.c_str());

		auto testcombo = nn::hid::KEY_A;
		if(P2->InputHeld(testcombo))
			xenomods::debug::drawFontShadow(1280/2, 0, mm::Col4::cyan, "combo held!");
		if (P2->InputDown(testcombo))
			g_Logger->LogDebug("combo down...");
		if (P2->InputUp(testcombo))
			g_Logger->LogDebug("combo up!");
		if(P2->InputHeldStrict(testcombo))
			xenomods::debug::drawFontShadow(1280/2, 16, mm::Col4::cyan, "strict combo held!");
		if (P2->InputDownStrict(testcombo))
			g_Logger->LogDebug("strict combo down...");
		if (P2->InputUpStrict(testcombo))
			g_Logger->LogDebug("strict combo up!");*/

		/*
		 * Enforce some things on first update
		 */
		static bool hasUpdated;
		if(!hasUpdated) {
#if XENOMODS_OLD_ENGINE
			fw::PadManager::enableDebugDraw(true);
#endif
			hasUpdated = true;
		}

		/*
		 * Check buttons
		 */

		if(P2->InputDownStrict(DISPLAY_VERSION)) {
			toastVersion();
		} else if(P2->InputDownStrict(RELOAD_CONFIG)) {
			GetState().config.LoadFromFile();
#if !XENOMODS_CODENAME(bf3)
			DebugStuff::PlaySE(gf::GfMenuObjUtil::SEIndex::Sort);
#endif
		} else if(P2->InputDownStrict(LOGGER_TEST)) {
			g_Logger->LogDebug("test debug message! {}", nn::os::GetSystemTick() / 19200000.);
			g_Logger->LogInfo("test info message! {}", nn::os::GetSystemTick() / 19200000.);
			g_Logger->LogWarning("test warning message! {}", nn::os::GetSystemTick() / 19200000.);
			g_Logger->LogError("test error message! {}", nn::os::GetSystemTick() / 19200000.);
			g_Logger->LogFatal("test fatal message! {}", nn::os::GetSystemTick());

			int group = nn::os::GetSystemTick() % 9;
			g_Logger->ToastWarning(fmt::format("{}", group), "random group ({})", group);
			g_Logger->ToastMessage("logger test", Logger::Severity::Info, "system tick in seconds: {:2f}", nn::os::GetSystemTick() / 19200000.);
		}

		// Update modules
		xenomods::UpdateAllRegisteredModules(updateInfo);

		// draw log messages
		g_Logger->Draw(updateInfo);
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

#if XENOMODS_CODENAME(bfsw)
		EnableDebugDrawing::HookAt(&ml::Scn::callbackToListener);
#elif XENOMODS_CODENAME(bf3)
		EnableDebugDrawing::HookFromBase(0x126dd38);
#endif

		// hook our updater
#if XENOMODS_OLD_ENGINE
		FrameworkUpdateHook::HookAt("_ZN2fw9Framework6updateEv");
#elif XENOMODS_CODENAME(bfsw)
		FrameworkUpdater_updateStdHook::HookAt("_ZN2fw16FrameworkUpdater9updateStdERKNS_8DocumentEPNS_19FrameworkControllerE");
#elif XENOMODS_CODENAME(bf3)
		FrameworkUpdater_updateStdHook::HookFromBase(0x710006734c);
#endif

		// Multiple controller support
		NpadStyleSetOverride::HookAt(&nn::hid::SetSupportedNpadStyleSet);
		DisableControllerSingleMode::HookAt(nn::hid::ShowControllerSupport);
#if !XENOMODS_CODENAME(bf3)
		ClampNumberOfControllers::HookAt("_ZN2ml8DevPadNx23getLocalConnectPadCountEv");
#else
		ClampNumberOfControllers::HookFromBase(0x7101251bcc);
#endif

		toastVersion();
	}

} // namespace xenomods
