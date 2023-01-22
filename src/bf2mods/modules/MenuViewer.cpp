#include "MenuViewer.hpp"

#include <bf2mods/DebugWrappers.hpp>
#include <bf2mods/HidInput.hpp>
#include <bf2mods/Logger.hpp>
#include <skylaunch/hookng/Hooks.hpp>

#include "../State.hpp"
#include "../main.hpp"
#include "bf2mods/engine/gf/MenuObject.hpp"
#include "bf2mods/engine/layer/LayerObj.hpp"
#include "bf2mods/engine/ui/UIObjectAcc.hpp"
#include "version.h"

namespace {

	struct SkipLayerRendering : skylaunch::hook::Trampoline<SkipLayerRendering> {
		static void Hook(void* this_pointer, void* IDrDrawWorkInfo) {
			if(bf2mods::MenuViewer::enableUIRendering)
				Orig(this_pointer, IDrDrawWorkInfo);
		}
	};

	struct MainMenuVersionInfo : skylaunch::hook::Trampoline<MainMenuVersionInfo> {
		static void Hook(gf::GfMenuObjTitle* this_pointer) {
			Orig(this_pointer);

			ui::UIObjectAcc titlescreen = ui::UIObjectAcc(this_pointer->rootUIObjAcc);
			ui::UIObjectAcc versionNum = ui::UIObjectAcc(this_pointer->rootUIObjAcc, "TXT_version_num");

			if (versionNum.objectId == 0)
				return;

			// we're duplicating the copyright text for its length
			// for whatever reason we're still bounded by the size of the ui object,
			// and it doesn't seem like there's a clear way to resize it properly
			auto newVersionId = titlescreen.duplicateChild("TXT_copyright");
			ui::UIObjectAcc newVersion = ui::UIObjectAcc(newVersionId);

			if (newVersion.objectId == 0)
				return;

			// make the version string...
			std::string version = fmt::format("bf2mods {}", bf2mods::version::tagDirty);
			auto newText = ui::UIStr(version.c_str(), true);
			newVersion.setText(newText);

			// i want the offset the game version has
			mm::Rect<short> rect;
			versionNum.getRect(rect, 2);
			short offset = 1280 - rect.w - rect.x;
			newVersion.getRect(rect, 2);

			// so we can use it for ourselves
			mm::Pnt<short> pos;
			versionNum.getPos(pos);
			pos.y += 25;
			pos.x = 1280 - offset - rect.w;
			newVersion.setPos(pos);
		}
	};

	struct StraightensYourXenoblade : skylaunch::hook::Trampoline<StraightensYourXenoblade> {
		static void Hook(layer::LayerObjFont* this_pointer, void* LayerRenderView, void* LayerResMatrix, void* LayerResColor) {
			float temp = this_pointer->slopeRot;
			if (bf2mods::MenuViewer::straightenFont)
				this_pointer->slopeRot = 0; // hook to the system tick for fun times
			Orig(this_pointer, LayerRenderView, LayerResMatrix, LayerResColor);
			this_pointer->slopeRot = temp;
		}
	};

}

namespace bf2mods {

	bool MenuViewer::enableUIRendering = true;
	bool MenuViewer::straightenFont = false;

	void MenuViewer::Initialize() {
		g_Logger->LogDebug("Setting up menu viewer...");

		SkipLayerRendering::HookAt("_ZN5layer12LayerManager11finalRenderEPKN2ml15IDrDrawWorkInfoE");

#if !BF2MODS_CODENAME(bfsw)
		MainMenuVersionInfo::HookAt(&gf::GfMenuObjTitle::initialize);

		// doesn't seem to work in DE, but does at least hook
		StraightensYourXenoblade::HookAt("_ZN5layer12LayerObjFont17updateShaderParmsEPKNS_15LayerRenderViewERKNS_14LayerResMatrixERKNS_13LayerResColorE");
#endif
	}

	void MenuViewer::Update() {
		auto& state = GetState();

		if(GetPlayer(2)->InputDownStrict(Keybind::UI_RENDER_TOGGLE)) {
			enableUIRendering = !enableUIRendering;
			g_Logger->LogInfo("UI rendering: {}", enableUIRendering);
		}
		else if (GetPlayer(2)->InputDownStrict(Keybind::STRAIGHTEN_FONT)) {
			straightenFont = !straightenFont;
		}
	}

	BF2MODS_REGISTER_MODULE(MenuViewer);

} // namespace bf2mods
