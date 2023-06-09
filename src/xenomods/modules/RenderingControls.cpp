#include "RenderingControls.hpp"

#include <skylaunch/hookng/Hooks.hpp>
#include <xenomods/DebugWrappers.hpp>
#include <xenomods/HidInput.hpp>
#include <xenomods/Logger.hpp>
#include <xenomods/State.hpp>
#include <xenomods/Version.hpp>
#include <xenomods/menu/Menu.hpp>

#include "DebugStuff.hpp"
#include "xenomods/engine/gf/MenuObject.hpp"
#include "xenomods/engine/layer/LayerManager.hpp"
#include "xenomods/engine/layer/LayerObj.hpp"
#include "xenomods/engine/ui/UIObjectAcc.hpp"

namespace {

	struct SkipLayerRendering : skylaunch::hook::Trampoline<SkipLayerRendering> {
		static void Hook(layer::LayerManager* this_pointer, ml::IDrDrawWorkInfo* info) {
			if(!xenomods::RenderingControls::skipUIRendering)
				Orig(this_pointer, info);
		}
	};

	struct SkipLayer2Rendering : skylaunch::hook::Trampoline<SkipLayer2Rendering> {
		static void Hook(layer::LayerManager* this_pointer, ml::IDrDrawWorkInfo* info) {
			if(!xenomods::RenderingControls::skipUIRendering)
				Orig(this_pointer, info);
		}
	};

	struct SkipCloudRendering : skylaunch::hook::Trampoline<SkipCloudRendering> {
		static void Hook(void* this_pointer, ml::IDrDrawWorkInfo* param_1, void* DrMdoZSortMan) {
			if(!xenomods::RenderingControls::skipCloudRendering)
				Orig(this_pointer, param_1, DrMdoZSortMan);
		}
	};

	struct SkipSkyDomeRendering : skylaunch::hook::Trampoline<SkipSkyDomeRendering> {
		static void Hook(void* this_pointer) {
			if(!xenomods::RenderingControls::skipSkyDomeRendering)
				Orig(this_pointer);
		}
	};

#if XENOMODS_OLD_ENGINE
	struct SkipParticleRendering : skylaunch::hook::Trampoline<SkipParticleRendering> {
		static void Hook(void* this_pointer, void* DrDrawWorkInfoEF, int param_2, bool param_3) {
			if(!xenomods::RenderingControls::skipParticleRendering)
				Orig(this_pointer, DrDrawWorkInfoEF, param_2, param_3);
		}
	};
#elif XENOMODS_CODENAME(bfsw)
	struct SkipParticleRendering : skylaunch::hook::Trampoline<SkipParticleRendering> {
		static void Hook(void* this_pointer) {
			if(!xenomods::RenderingControls::skipParticleRendering)
				Orig(this_pointer);
		}
	};
#endif

#if XENOMODS_OLD_ENGINE
	struct StraightensYourXenoblade : skylaunch::hook::Trampoline<StraightensYourXenoblade> {
		static void Hook(layer::LayerObjFont* this_pointer, void* LayerRenderView, void* LayerResMatrix, void* LayerResColor) {
			float temp = this_pointer->slopeRot;
			if (xenomods::RenderingControls::straightenFont)
				this_pointer->slopeRot = 0; // hook to the system tick for fun times
			Orig(this_pointer, LayerRenderView, LayerResMatrix, LayerResColor);
			this_pointer->slopeRot = temp;
		}
	};
#endif

}

namespace xenomods {

	RenderingControls::ForcedRenderParameters RenderingControls::ForcedParameters = {};

	bool RenderingControls::skipUIRendering = false;
	bool RenderingControls::skipParticleRendering = false;
	bool RenderingControls::skipCloudRendering = false;
	bool RenderingControls::skipSkyDomeRendering = false;
	bool RenderingControls::straightenFont = false;

	const std::string toggleKey = std::string(STRINGIFY(RenderingControls)) + "_Toggles";

	void MenuToggleMap() {
		auto acc = ml::ScnRenderDrSysParmAcc();
		// done this way because 2/Torna do not have is/setDispMap
		acc.drMan->hideMap = !acc.drMan->hideMap;
		g_Logger->ToastInfo(toggleKey, "Toggled map: {}", !acc.drMan->hideMap);
	}
	void MenuToggleFog() {
		auto acc = ml::ScnRenderDrSysParmAcc();
		static bool fogSkip;
		fogSkip = !fogSkip;
		acc.setFogSkip(fogSkip);
		g_Logger->ToastInfo(toggleKey, "Toggled fog: {}", !fogSkip);
	}
	void MenuToggleBloom() {
		auto acc = ml::ScnRenderDrSysParmAcc();
		acc.setBloom(!acc.isBloomOn());
		g_Logger->ToastInfo(toggleKey, "Toggled bloom: {}", acc.isBloomOn());
	}
	void MenuToggleTonemapping() {
		auto acc = ml::ScnRenderDrSysParmAcc();
		acc.setToneMap(!acc.isToneMap());
		g_Logger->ToastInfo(toggleKey, "Toggled tone mapping: {}", acc.isToneMap());
	}
	void MenuToggleDOF() {
		RenderingControls::ForcedParameters.DisableDOF = !RenderingControls::ForcedParameters.DisableDOF;
		g_Logger->ToastInfo(toggleKey, "Toggled depth of field: {}", !RenderingControls::ForcedParameters.DisableDOF);
	}
	void MenuToggleMotionBlur() {
		RenderingControls::ForcedParameters.DisableMotionBlur = !RenderingControls::ForcedParameters.DisableMotionBlur;
		g_Logger->ToastInfo(toggleKey, "Toggled motion blur: {}", !RenderingControls::ForcedParameters.DisableMotionBlur);
	}
	void MenuToggleColorFilter() {
		RenderingControls::ForcedParameters.DisableColorFilter = !RenderingControls::ForcedParameters.DisableColorFilter;
		g_Logger->ToastInfo(toggleKey, "Toggled color filter: {}", !RenderingControls::ForcedParameters.DisableColorFilter);
	}

	void RenderingControls::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up rendering controls...");

#if !XENOMODS_CODENAME(bf3)
		SkipLayerRendering::HookAt("_ZN5layer12LayerManager11finalRenderEPKN2ml15IDrDrawWorkInfoE");
		SkipCloudRendering::HookAt("_ZN5cloud8CloudMan5applyEPKN2ml15IDrDrawWorkInfoEPNS1_13DrMdoZSortManE");
		SkipSkyDomeRendering::HookAt("_ZN2ml9DrPixlMan13renderSkyDomeEv");
#else
		SkipLayerRendering::HookFromBase(0x710100f790);
		SkipLayer2Rendering::HookFromBase(0x710100f808);
#endif

#if XENOMODS_OLD_ENGINE
		StraightensYourXenoblade::HookAt("_ZN5layer12LayerObjFont17updateShaderParmsEPKNS_15LayerRenderViewERKNS_14LayerResMatrixERKNS_13LayerResColorE");
		SkipParticleRendering::HookAt("_ZN5ptlib15ParticleManager4drawEPKNS_16DrDrawWorkInfoEFEib");
#elif XENOMODS_CODENAME(bfsw)
		SkipParticleRendering::HookAt("_ZN4xefb9CEEmitter4drawEv");
#endif

		auto modules = g_Menu->FindSection("modules");
		if (modules != nullptr) {
			auto section = modules->RegisterSection(STRINGIFY(RenderingControls), "Rendering Controls");

#if XENOMODS_OLD_ENGINE
			section->RegisterOption<bool>(straightenFont, "Straighten font");
#endif
			section->RegisterOption<bool>(skipUIRendering, "Skip UI rendering");
#if !XENOMODS_CODENAME(bf3)
			section->RegisterOption<bool>(skipParticleRendering, "Skip particle+overlay rendering");
			section->RegisterOption<bool>(skipCloudRendering, "Skip cloud (sea) rendering");
			section->RegisterOption<bool>(skipSkyDomeRendering, "Skip sky dome rendering");
#endif

#if !XENOMODS_CODENAME(bf3)
			auto toggles = section->RegisterSection(toggleKey, "Toggles...");
			toggles->RegisterOption<void>("Toggle map", &MenuToggleMap);
			toggles->RegisterOption<void>("Toggle fog", &MenuToggleFog);
			toggles->RegisterOption<void>("Toggle bloom", &MenuToggleBloom);
			toggles->RegisterOption<void>("Toggle tonemapping", &MenuToggleTonemapping);
			toggles->RegisterOption<void>("Toggle depth of field", &MenuToggleDOF);
			toggles->RegisterOption<void>("Toggle motion blur", &MenuToggleMotionBlur);
			toggles->RegisterOption<void>("Toggle color filter", &MenuToggleColorFilter);
#endif
		}
	}

	void RenderingControls::Update(fw::UpdateInfo* updateInfo) {
		UpdatableModule::Update(updateInfo);

#if !XENOMODS_CODENAME(bf3)
		if (ForcedParameters.Any()) {
			auto acc = ml::ScnRenderDrSysParmAcc();

			if (ForcedParameters.DisableDOF) {
				acc.setDOFOverride(true);
				acc.setDOF(false);
			}
			if (ForcedParameters.DisableMotionBlur) {
				acc.setMotBlurOverride(true);
				acc.setMotBlur(false);
			}
			if (ForcedParameters.DisableColorFilter) {
				acc.setColorFilterOverride(true);
				acc.setColorFilterNum(0);
				acc.setColorFilterFarNum(0);
				acc.setColorFilterFrm(0);
			}
		}
#endif
	}

	XENOMODS_REGISTER_MODULE(RenderingControls);

} // namespace xenomods
