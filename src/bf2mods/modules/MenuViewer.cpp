#include "MenuViewer.hpp"
#include "DebugStuff.hpp"

#include <bf2mods/DebugWrappers.hpp>
#include <bf2mods/HidInput.hpp>
#include <bf2mods/Logger.hpp>
#include <skylaunch/hookng/Hooks.hpp>

#include "../State.hpp"
#include "../main.hpp"
#include "bf2mods/engine/gf/MenuObject.hpp"
#include "bf2mods/engine/layer/LayerObj.hpp"
#include "bf2mods/engine/ui/UIObjectAcc.hpp"
#include "gitversion.h"

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
			if (versionNum.uiObject == nullptr)
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
			if (modVersion.uiObject == nullptr)
				return;

			modVersion.uiObject->name.set("TXT_mod_version");

			// make the version string...
			auto modVersionStr = fmt::format("bf2mods {}", bf2mods::version::fullTag);
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
			if (modBuildDate.uiObject == nullptr)
				return;

			modBuildDate.uiObject->name.set("TXT_mod_builddate");

			auto modBuildDateStr = fmt::format("Built {} {}", __DATE__, __TIME__);
			auto modBuildDateUIStr = ui::UIStr(modBuildDateStr.c_str(), true);
			modBuildDate.setText(modBuildDateUIStr);

			modBuildDate.getRect(scratchRect, 2);
			scratchPos.x = 1280 - offsetX - scratchRect.w;
			scratchPos.y = offsetY += 2;
			offsetY += scratchRect.h;
			modBuildDate.setPos(scratchPos);
#endif

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

	struct BigOlUIDebugger : skylaunch::hook::Trampoline<BigOlUIDebugger> {
		static void Hook(ui::UIObject* this_pointer) {
			Orig(this_pointer);

			if (!bf2mods::DebugStuff::enableDebugRendering)
				return;

			bool shouldSkip = false;
			bool goodName = false;

			ui::UIObject* obj = this_pointer;
			std::string_view name = {obj->name.buffer, obj->name.m_nLen};

			do {
				if (name.find("TXT_") != std::string::npos)
					goodName = true;

				shouldSkip = !obj->displayInfo.isDisp;
				obj = obj->getParentPtr();
			}
			while (!shouldSkip && obj != nullptr);

			if (!goodName || shouldSkip)
				return;

			mm::Rect<short> rectS;
			auto objAcc = ui::UIObjectAcc(this_pointer->objectId);
			objAcc.getRect(rectS, 0b11);

			mm::Rect<int> rect = {rectS.x, rectS.y, rectS.w, rectS.h};
			fw::debug::drawLineSquare2D(rect, mm::Col4::white);

			short height = fw::debug::drawFontGetHeight();
			short textX = rect.x;
			short textY = rect.y + rect.h - height;

			mm::Col4 transparentWhite = {1, 1, 1, 0.8f};

			fw::debug::drawFontFmt(textX, textY += 0, transparentWhite, "{}", this_pointer->name.buffer);
			//fw::debug::drawFontFmt(textX, textY += height, mm::Col4::white, "flags: {}", static_cast<uint>(this_pointer->flags));

			if (this_pointer->getRTTI()->isKindOf(&ui::UITextObject::m_rtti)) {
				//fw::debug::drawFontFmt(textX, textY += height, transparentWhite, "alignH: {}", objAcc.getTextAlignH());
				//fw::debug::drawFontFmt(textX, textY += height, transparentWhite, "alignV: {}", objAcc.getTextAlignV());
				//fw::debug::drawFontFmt(textX, textY += height, transparentWhite, "dispmode: {}", static_cast<byte>(this_pointer->displayInfo.displayMode));
			}

			/*ui::UIObjectPlugin* plugin = this_pointer->latestPlugin;
			while (plugin != nullptr) {
				fw::debug::drawFontFmt(textX, textY += height, transparentWhite, "Plugin: {}", plugin->getRTTI()->szName);
				plugin = plugin->prevPlugin;
			}*/
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

		//BigOlUIDebugger::HookAt("_ZN2ui8UIObject6updateEv");

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
