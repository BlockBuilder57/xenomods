#include "MenuViewer.hpp"

#include "../State.hpp"
#include "../main.hpp"
#include <bf2mods/Logger.hpp>
#include <skylaunch/hookng/Hooks.hpp>

namespace {

	struct SkipLayerRendering : skylaunch::hook::Trampoline<SkipLayerRendering> {
		static void Hook(void* this_pointer, void* IDrDrawWorkInfo) {
			if(bf2mods::MenuViewer::enableUIRendering)
				Orig(this_pointer, IDrDrawWorkInfo);
		}
	};

}

namespace bf2mods {

	bool MenuViewer::enableUIRendering = true;

	void MenuViewer::Initialize() {
		g_Logger->LogDebug("Setting up menu viewer...");

		SkipLayerRendering::HookAt("_ZN5layer12LayerManager11finalRenderEPKN2ml15IDrDrawWorkInfoE");
	}

	void MenuViewer::Update() {
		auto& state = GetState();

		if(btnDown(Keybind::UI_RENDER_TOGGLE, p2Cur.Buttons, p2Prev.Buttons)) {
			enableUIRendering = !enableUIRendering;
			g_Logger->LogInfo("UI rendering: {}", enableUIRendering);
		}
	}

	BF2MODS_REGISTER_MODULE(MenuViewer);

} // namespace bf2mods
