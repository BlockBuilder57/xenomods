#include "MenuViewer.hpp"

#include "../State.hpp"
#include "../main.hpp"
#include "bf2mods/Logger.hpp"
#include "bf2mods/stuff/utils/util.hpp"
#include "skylaunch/logger/Logger.hpp"

namespace gf {

	class GfMenuTypes {
	   public:
		struct Param {
			uintptr_t field_0x0;
			uintptr_t field_0x8;
			uintptr_t field_0x10;
			uintptr_t field_0x18;
		};
	};

	/*GENERATE_SYM_HOOK(GfMenuManager_getGameVersionString, "_ZN2gf13GfMenuManager20getGameVersionStringEv", char*) {
		// I bought your Colgate toothpaste, the one with tartar control, AND IT MADE ME FEEL LIKE A PIECE OF SHIT!
		static constexpr char a[] = "Colgate";
		return &a[0];
	}*/

	GENERATE_SYM_HOOK(GfMenuFullScreen_registLayerParam, "_ZN2gf16GfMenuFullScreen16registLayerParamEjRKNS_11GfMenuTypes5ParamE", void, unsigned int param_1, GfMenuTypes::Param* param_2) {
		bf2mods::g_Logger->LogInfo("registLayerParam(uint: {}, Param: [{:p}, {:p}, {:p}, {:p}]", param_1, param_2->field_0x0, param_2->field_0x8, param_2->field_0x10, param_2->field_0x18);
		GfMenuFullScreen_registLayerParamBak(param_1, param_2);
	}
	GENERATE_SYM_HOOK(GfMenuFullScreen_registLayerObject, "_ZN2gf16GfMenuFullScreen17registLayerObjectEjj", void, unsigned int param_1, unsigned int param_2) {
		bf2mods::g_Logger->LogInfo("registLayerObject({}, {})", param_1, param_2);
		GfMenuFullScreen_registLayerObjectBak(param_1, param_2);
	}

	struct GfMenuObjCommonJog {
		static void (*ctor)(void* this_pointer, unsigned int param_1);
	};

	template<typename T>
	void** GetVTable(T* obj) {
		return *((void***)obj);
	}

	alignas(1) std::uint8_t JogInstance[0x7f]; // jog

	/*
	GENERATE_SYM_HOOK(GfMenuObjWorldMapCtor, "_ZN2gf17GfMenuObjWorldMapC2Ej", void, void* this_pointer, unsigned int param_1) {
		//GfMenuObjCommonJog::ctor(this_pointer, param_1);
		bf2mods::g_Logger->LogInfo("before ctor");
		GfMenuObjWorldMapCtorBak(this_pointer, param_1);
		bf2mods::g_Logger->LogInfo("after ctor");
		// attempt to set the vtable to GfMenuObjCommonJog's

		//bf2mods::g_Logger->LogInfo("before vtable set");
		//auto vtable_ptr = reinterpret_cast<void**>(this_pointer);
		//*vtable_ptr = reinterpret_cast<void*>(0x710135aa20);
		//bf2mods::g_Logger->LogInfo("please don't crash");
	}
	 */

} // namespace gf

namespace layer {

	GENERATE_SYM_HOOK(LayerManager_finalRender, "_ZN5layer12LayerManager11finalRenderEPKN2ml15IDrDrawWorkInfoE", void, void* this_pointer, void* IDrDrawWorkInfo) {
		if(bf2mods::MenuViewer::enableUIRendering)
			LayerManager_finalRenderBak(this_pointer, IDrDrawWorkInfo);
	}

} // namespace layer

namespace bf2mods {

	bool MenuViewer::enableUIRendering = true;

	void MenuViewer::Initialize() {
		g_Logger->LogDebug("Setting up menu viewer...");

		//gf::GfMenuManager_getGameVersionStringHook();
		//gf::GfMenuFullScreen_registLayerParamHook();
		//gf::GfMenuFullScreen_registLayerObjectHook();

		//util::ResolveSymbol<decltype(gf::GfMenuObjCommonJog::ctor)>(&gf::GfMenuObjCommonJog::ctor, "_ZN2gf17GfMenuObjWorldMapC1Ej");
		//gf::GfMenuObjWorldMapCtorHook();

		//ui::UIManager_createLayerHook();
		//ui::UIManager_releaseLayerHook();
		//ui::UIManager_terminateLayerHook();

		layer::LayerManager_finalRenderHook();
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
