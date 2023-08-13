// Created by block on 6/17/23.

#include "FunctionalHooks.hpp"

#include <skylaunch/hookng/Hooks.hpp>
#include <xenomods/DebugWrappers.hpp>
#include <xenomods/HidInput.hpp>
#include <xenomods/Logger.hpp>
#include <xenomods/NnFile.hpp>
#include <xenomods/State.hpp>
#include <xenomods/Version.hpp>
#include <xenomods/menu/Menu.hpp>

#include "main.hpp"
#include "xenomods/engine/fw/Document.hpp"
#include "xenomods/engine/fw/Framework.hpp"
#include "xenomods/engine/fw/Managers.hpp"
#include "xenomods/engine/game/Sequence.hpp"
#include "xenomods/engine/gf/BdatData.hpp"
#include "xenomods/engine/gf/Manager.hpp"
#include "xenomods/engine/gf/MenuObject.hpp"
#include "xenomods/engine/layer/LayerManager.hpp"
#include "xenomods/engine/layer/LayerObj.hpp"
#include "xenomods/engine/ml/DevGraph.hpp"
#include "xenomods/engine/ml/Filesystem.hpp"
#include "xenomods/engine/ml/Rand.hpp"
#include "xenomods/engine/ui/UIObjectAcc.hpp"

namespace {

#if XENOMODS_OLD_ENGINE
	struct FrameworkUpdateHook : skylaunch::hook::Trampoline<FrameworkUpdateHook> {
		static void Hook(fw::Framework* framework) {
			Orig(framework);

			if(framework != nullptr)
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
			((void (*)(unsigned int))(skylaunch::utils::g_MainTextAddr + 0x1248774))(0);
			((void (*)(int, const mm::Mat44&, const mm::Mat44&))(skylaunch::utils::g_MainTextAddr + 0x124bdec))(-1, identity, identity);
			((void (*)())(skylaunch::utils::g_MainTextAddr + 0x1248798))();
		}
	};
#endif

#if XENOMODS_OLD_ENGINE
	struct MainMenuVersionInfo : skylaunch::hook::Trampoline<MainMenuVersionInfo> {
		static void Hook(gf::GfMenuObjTitle* this_pointer) {
			Orig(this_pointer);

			ui::UIObjectAcc titlescreen = ui::UIObjectAcc(this_pointer->rootUIObjAcc);
			ui::UIObjectAcc versionNum = ui::UIObjectAcc(this_pointer->rootUIObjAcc, "TXT_version_num");
			if(versionNum.uiObject == nullptr)
				return;

			// get the offset the game version has
			mm::Rect<short> versionRect;
			mm::Rect<short> scratchRect;
			mm::Pnt<short> scratchPos;

			versionNum.getRect(versionRect, 2);
			short offsetX = 1280 - versionRect.w - versionRect.x;
			short offsetY = versionRect.y + versionRect.h;

			// we're duplicating the copyright text for its length
			// for whatever reason we're still bounded by some size of the ui object,
			// and it doesn't seem like there's a clear way to change it properly
			ui::UIObjectAcc modVersion = ui::UIObjectAcc(titlescreen.duplicateChild("TXT_copyright"));
			if(modVersion.uiObject == nullptr)
				return;

			modVersion.uiObject->name.set("TXT_mod_version");

			// make the version string...
			auto modVersionStr = fmt::format("xenomods {}", xenomods::version::BuildGitVersion());
			auto modVersionUIStr = ui::UIStr(modVersionStr.c_str(), true);
			modVersion.setText(modVersionUIStr);

			// now we can use the offset for ourselves
			modVersion.getRect(scratchRect, 2);
			scratchPos.x = 1280 - offsetX - scratchRect.w;
			scratchPos.y = offsetY += 2;
			offsetY += scratchRect.h;
			modVersion.setPos(scratchPos);

	#if _DEBUG
			// might as well show a build string too
			// ideally this would just be a newline in the version string,
			// but it seems that fixing the text size is more hassle than its worth

			ui::UIObjectAcc modBuildDate = ui::UIObjectAcc(titlescreen.duplicateChild("TXT_copyright"));
			if(modBuildDate.uiObject == nullptr)
				return;

			modBuildDate.uiObject->name.set("TXT_mod_builddate");

			auto modBuildDateUIStr = ui::UIStr(xenomods::version::BuildTimestamp(), true);
			modBuildDate.setText(modBuildDateUIStr);

			modBuildDate.getRect(scratchRect, 2);
			scratchPos.x = 1280 - offsetX - scratchRect.w;
			scratchPos.y = offsetY += 2;
			offsetY += scratchRect.h;
			modBuildDate.setPos(scratchPos);
	#endif
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
			nn::hid::GetNpadState(&p1State, 0);

			if(p1State.mAttributes.isBitSet(nn::hid::NpadAttribute::IsConnected)) {
				// We have a player 1 at this point, so disable single-controller only mode
				// That way if another controller is connected while the game is running, we can allow it to be P2
				// Monolib actually passes the (according to switchbrew) default arguments to this,
				// so we technically get 4 max controllers for free!
				supportArg->mSingleMode = false;
			}

			Orig(resultInfo, supportArg);
		}
	};


	struct DisableControllerUpdatingForMenu : skylaunch::hook::Trampoline<DisableControllerUpdatingForMenu> {
		static void Hook(void* this_pointer) {
			if(xenomods::ClampNumberOfControllers::HasApplied() && xenomods::ClampNumberOfControllers::Orig() > 1)
				return Orig(this_pointer);

			if(!xenomods::g_Menu->IsOpen())
				return Orig(this_pointer);
		}
	};

#if XENOMODS_OLD_ENGINE
	struct OnMapChange : skylaunch::hook::Trampoline<OnMapChange> {
		static void Hook(uint* GfReqParam) {
			Orig(GfReqParam);

			unsigned int mapId = gf::GfDataMap::getMapID(*GfReqParam);

			xenomods::MapChangeForAllRegisteredModules(mapId);
		}
	};
#elif XENOMODS_CODENAME(bfsw)
	struct OnMapChange : skylaunch::hook::Trampoline<OnMapChange> {
		static void Hook(game::SeqMapJump* this_pointer, game::MapJumpSetupInfo* setupInfo) {
			Orig(this_pointer, setupInfo);

			std::string thingy = fmt::format("ma{:02}{:02}", setupInfo->chapter, setupInfo->location);
			unsigned int mapId = game::DataUtil::searchBdatFldMaplistRowID(*xenomods::DocumentPtr, thingy.c_str());

			xenomods::MapChangeForAllRegisteredModules(mapId);
		}
	};
#endif

}; // namespace

namespace xenomods {

	// Monolib gets very upset in 3 when the controller count is above 1, so let's clamp that down
	int ClampNumberOfControllers::Hook() {
		int result = Orig();

		if(result > 1)
			return 1;

		return result;
	}

#if XENOMODS_NEW_ENGINE
	fw::Document* DocumentPtr = {};
#endif

	void FunctionalHooks::Hook() {
#if XENOMODS_OLD_ENGINE
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
		MainMenuVersionInfo::HookAt(&gf::GfMenuObjTitle::initialize);
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

		// Disable inputs from P1 when the Menu is open
#if !XENOMODS_CODENAME(bf3)
		DisableControllerUpdatingForMenu::HookAt("_ZN2ml8DevPadNx4Impl10updateNpadEv");
#else
		DisableControllerUpdatingForMenu::HookFromBase(0x710124fa34);
#endif

		// Map change events
#if XENOMODS_OLD_ENGINE
		OnMapChange::HookAt("_ZN2gf12GfReqCommand11execMapjumpEPKNS_10GfReqParamE");
#elif XENOMODS_CODENAME(bfsw)
		OnMapChange::HookAt("_ZN4game10SeqMapJumpC1ERKNS_16MapJumpSetupInfoE");
#endif

	}

} // namespace xenomods