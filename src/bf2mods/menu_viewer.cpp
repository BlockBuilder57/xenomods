#include "menu_viewer.hpp"

#include "bf2logger.hpp"
#include "bf2mods/stuff/utils/util.hpp"
#include "plugin.hpp"
#include "skyline/logger/Logger.hpp"

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
		bf2mods::g_Logger->LogInfo("registLayerParam(uint: %u, Param: [%p, %p, %p, %p]", param_1, param_2->field_0x0, param_2->field_0x8, param_2->field_0x10, param_2->field_0x18);
		GfMenuFullScreen_registLayerParamBak(param_1, param_2);
	}
	GENERATE_SYM_HOOK(GfMenuFullScreen_registLayerObject, "_ZN2gf16GfMenuFullScreen17registLayerObjectEjj", void, unsigned int param_1, unsigned int param_2) {
		bf2mods::g_Logger->LogInfo("registLayerObject(%u, %u)", param_1, param_2);
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

namespace ui {

	GENERATE_SYM_HOOK(UIManager_createLayer, "_ZN2ui9UIManager11createLayerEj", void*, void* this_pointer, unsigned int param_1) {
		bf2mods::g_Logger->LogInfo("UIManager::createLayer(%p, %u)", this_pointer, param_1);
		bf2mods::MenuViewer::UIManagerPtr = this_pointer;
		return UIManager_createLayerBak(this_pointer, param_1);
	}

	GENERATE_SYM_HOOK(UIManager_releaseLayer, "_ZN2ui9UIManager12releaseLayerEj", void, void* this_pointer, unsigned int param_1) {
		bf2mods::g_Logger->LogInfo("UIManager::releaseLayer(%p, %u)", this_pointer, param_1);
		bf2mods::MenuViewer::UIManagerPtr = this_pointer;
		UIManager_releaseLayerBak(this_pointer, param_1);
	}
	GENERATE_SYM_HOOK(UIManager_terminateLayer, "_ZN2ui9UIManager14terminateLayerEjb", void, void* this_pointer, unsigned int param_1, bool param_2) {
		bf2mods::g_Logger->LogInfo("UIManager::terminateLayer(%p, %u, %s)", this_pointer, param_1, bf2mods::format(param_2).c_str());
		bf2mods::MenuViewer::UIManagerPtr = this_pointer;
		UIManager_terminateLayerBak(this_pointer, param_1, param_2);
	}

} // namespace ui

namespace bf2mods::MenuViewer {

	void* UIManagerPtr;

	void OpenLayer(unsigned int layer) {
		if(UIManagerPtr != nullptr)
			ui::UIManager_createLayerBak(UIManagerPtr, layer);
	}
	void CloseLayer(unsigned int layer) {
		if(UIManagerPtr != nullptr) {
			ui::UIManager_releaseLayerBak(UIManagerPtr, layer);
			ui::UIManager_terminateLayerBak(UIManagerPtr, layer, false);
		}
	}

	void Setup() {
		g_Logger->LogInfo("Setting up menu viewer...");

		//gf::GfMenuManager_getGameVersionStringHook();
		//gf::GfMenuFullScreen_registLayerParamHook();
		//gf::GfMenuFullScreen_registLayerObjectHook();

		//util::ResolveSymbol<decltype(gf::GfMenuObjCommonJog::ctor)>(&gf::GfMenuObjCommonJog::ctor, "_ZN2gf17GfMenuObjWorldMapC1Ej");
		//gf::GfMenuObjWorldMapCtorHook();

		//ui::UIManager_createLayerHook();
		//ui::UIManager_releaseLayerHook();
		//ui::UIManager_terminateLayerHook();
	}

} // namespace bf2mods::MenuViewer
