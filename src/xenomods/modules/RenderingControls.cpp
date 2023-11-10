#include "RenderingControls.hpp"

#include "DebugStuff.hpp"
#include "xenomods/engine/effect/SystemManager.hpp"
#include "xenomods/engine/fw/Managers.hpp"
#include "xenomods/engine/gf/MenuObject.hpp"
#include "xenomods/engine/layer/LayerManager.hpp"
#include "xenomods/engine/layer/LayerObj.hpp"
#include "xenomods/engine/mm/mtl/PtrSingleton.hpp"
#include "xenomods/engine/ptlib/Emitter.hpp"
#include "xenomods/engine/ui/UIObjectAcc.hpp"
#include "xenomods/engine/xefb/Effect.hpp"

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

	struct FreezeTextureStreaming : skylaunch::hook::Trampoline<FreezeTextureStreaming> {
		static void Hook(ml::DrResMdoTexList* this_pointer) {
			if (!xenomods::RenderingControls::freezeTextureStreaming)
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
		static void Hook(xefb::CEParticlelist* this_pointer, xefb::CERes* res) {
			bool skip = false;

			using enum xefb::eXefbDrayType;
			switch(this_pointer->Emitter->drawType) {
				case PostLine:
				case PostGauss:
				case PostRadial:
				case PostMono:
				case PostDist: {
					if(xenomods::RenderingControls::skipOverlayRendering)
						skip = true;

					break;
				}
				default: {
					if(xenomods::RenderingControls::skipParticleRendering)
						skip = true;

					break;
				}
			}

			if(!skip)
				Orig(this_pointer, res);
		}
	};
#endif

#if XENOMODS_OLD_ENGINE
	struct StraightensYourXenoblade : skylaunch::hook::Trampoline<StraightensYourXenoblade> {
		static void Hook(layer::LayerObjFont* this_pointer, void* LayerRenderView, void* LayerResMatrix, void* LayerResColor) {
			float temp = this_pointer->slopeRot;
			if(xenomods::RenderingControls::straightenFont)
				this_pointer->slopeRot = 0; // hook to the system tick for fun times
			Orig(this_pointer, LayerRenderView, LayerResMatrix, LayerResColor);
			this_pointer->slopeRot = temp;
		}
	};
#endif

} // namespace

namespace xenomods {

	RenderingControls::ForcedRenderParameters RenderingControls::ForcedParameters = {};

	bool RenderingControls::straightenFont = false;
	bool RenderingControls::skipUIRendering = false;
	bool RenderingControls::skipParticleRendering = false;
	bool RenderingControls::skipOverlayRendering = false;
	bool RenderingControls::skipCloudRendering = false;
	bool RenderingControls::skipSkyDomeRendering = false;

	float RenderingControls::shadowStrength = 1.0;

	bool RenderingControls::freezeTextureStreaming = false;

	const std::string toggleKey = std::string(STRINGIFY(RenderingControls)) + "_Toggles";

	void RenderingControls::MenuSection() {
#if XENOMODS_OLD_ENGINE
		ImGui::Checkbox("Straighten font", &straightenFont);
#endif
#if !XENOMODS_CODENAME(bf3)
		ImGui::PushItemWidth(150.f);
		if(ImGui::SliderFloat("Shadow strength", &shadowStrength, -1, 1)) {
			auto acc = ml::ScnRenderDrSysParmAcc();
			acc.setShadowStr(shadowStrength);
		}
		ImGui::PopItemWidth();

		ImGui::Separator();

		if(ImGui::Button("Clear Texture Cache")) {
			auto ptr = *skylaunch::hook::detail::ResolveSymbol<ml::DrCalcTexStreamMan**>("_ZZN2mm3mtl12PtrSingletonIN2ml18DrCalcTexStreamManEE3sysEvE10s_instance");

			if (reinterpret_cast<uintptr_t>(ptr) != skylaunch::hook::INVALID_FUNCTION_PTR) {
				for (ml::DrCalcStmListObj* listObj = ptr->rootObjectIDK; listObj != nullptr; listObj = listObj->nextListObj) {
					g_Logger->LogDebug("Clearing texture cache for {}", listObj->path.buffer);
					listObj->chacheTexClear();
				}
			}
		}

		ImGui::Checkbox("Freeze texture streaming", &freezeTextureStreaming);
#endif
	}

	void RenderingControls::MenuToggles() {
		ImGui::Checkbox("Skip UI rendering", &skipUIRendering);
#if !XENOMODS_CODENAME(bf3)
	#if XENOMODS_OLD_ENGINE
		ImGui::Checkbox("Skip particle+overlay rendering", &skipParticleRendering);
	#else
		ImGui::Checkbox("Skip particle rendering", &skipParticleRendering);
		ImGui::Checkbox("Skip overlay rendering", &skipOverlayRendering);
	#endif
		ImGui::Checkbox("Skip cloud (sea) rendering", &skipCloudRendering);
		ImGui::Checkbox("Skip sky dome rendering", &skipSkyDomeRendering);

		auto acc = ml::ScnRenderDrSysParmAcc();
		static bool hideFog = false, hideBloom = false, hideTonemap = false;

		ImGui::Checkbox("Hide map", &acc.drMan->hideMap);
		if(ImGui::Checkbox("Disable fog", &hideFog))
			acc.setFogSkip(hideFog);
		if(ImGui::Checkbox("Disable bloom", &hideBloom))
			acc.setBloom(!hideBloom);
		if(ImGui::Checkbox("Disable tonemapping", &hideTonemap))
			acc.setToneMap(!hideTonemap);

		ImGui::Checkbox("Force disable depth of field", &ForcedParameters.DisableDOF);
		ImGui::Checkbox("Force disable motion blur", &ForcedParameters.DisableMotionBlur);
		ImGui::Checkbox("Force disable color filters", &ForcedParameters.DisableColorFilter);
		ImGui::Checkbox("Enable AA", &acc.PixlPostParm->enableAA);
		ImGui::Checkbox("Enable TMAA", &acc.PixlPostParm->enableTMAA);
#endif
	}

	void RenderingControls::MenuGBuffer() {
#if !XENOMODS_CODENAME(bf3)
		auto acc = ml::ScnRenderDrSysParmAcc();

		ImGui::Checkbox("GBuffer debug", &acc.PixlPostParm->GBufferDebug);
		ImGui::SliderFloat2("Base Color",        acc.PixlPostParm->GBufferDebugParams[0], 0, 1);
		ImGui::SliderFloat2("Metalness",         acc.PixlPostParm->GBufferDebugParams[1], 0, 1);
		ImGui::SliderFloat2("Roughness",         acc.PixlPostParm->GBufferDebugParams[2], 0, 1);
		ImGui::SliderFloat2("Emission",          acc.PixlPostParm->GBufferDebugParams[3], 0, 1);
		ImGui::SliderFloat2("N/A",               acc.PixlPostParm->GBufferDebugParams[4], 0, 1);
		ImGui::SliderFloat2("Ambient Occlusion", acc.PixlPostParm->GBufferDebugParams[5], 0, 1);
		ImGui::SliderFloat2("Emission 2",        acc.PixlPostParm->GBufferDebugParams[6], 0, 1);
		ImGui::SliderFloat2("Specular",          acc.PixlPostParm->GBufferDebugParams[7], 0, 1);

		if(ImGui::Button("Reset parameters"))
			acc.setGBuffDebugDefault();
#endif
	}

	void RenderingControls::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up rendering controls...");

#if !XENOMODS_CODENAME(bf3)
		SkipLayerRendering::HookAt("_ZN5layer12LayerManager11finalRenderEPKN2ml15IDrDrawWorkInfoE");
		SkipCloudRendering::HookAt("_ZN5cloud8CloudMan5applyEPKN2ml15IDrDrawWorkInfoEPNS1_13DrMdoZSortManE");
		SkipSkyDomeRendering::HookAt("_ZN2ml9DrPixlMan13renderSkyDomeEv");

		FreezeTextureStreaming::HookAt(&ml::DrResMdoTexList::texStmUpdate);
#else
		// layer::LayerManager::finalRender and its cousin
		if (version::RuntimeVersion() == version::SemVer::v2_0_0) {
			SkipLayerRendering::HookFromBase(0x710100f790);
			SkipLayer2Rendering::HookFromBase(0x710100f808);
		}
		else if (version::RuntimeVersion() == version::SemVer::v2_1_0) {
			SkipLayerRendering::HookFromBase(0x710100fac0);
			SkipLayer2Rendering::HookFromBase(0x710100fb38);
		}
		else if (version::RuntimeVersion() == version::SemVer::v2_1_1) {
			SkipLayerRendering::HookFromBase(0x710100fb00);
			SkipLayer2Rendering::HookFromBase(0x710100fb78);
		}
#endif

#if XENOMODS_OLD_ENGINE
		StraightensYourXenoblade::HookAt("_ZN5layer12LayerObjFont17updateShaderParmsEPKNS_15LayerRenderViewERKNS_14LayerResMatrixERKNS_13LayerResColorE");
		SkipParticleRendering::HookAt("_ZN5ptlib15ParticleManager4drawEPKNS_16DrDrawWorkInfoEFEib");
#elif XENOMODS_CODENAME(bfsw)
		SkipParticleRendering::HookAt("_ZN4xefb14CEParticlelist4drawEPNS_5CEResE");
#endif

		auto modules = g_Menu->FindSection("modules");
		if(modules != nullptr) {
			auto section = modules->RegisterSection(STRINGIFY(RenderingControls), "Rendering Controls");
			section->RegisterRenderCallback(&MenuSection);

			auto toggles = section->RegisterSection(toggleKey, "Toggles...");
			toggles->RegisterRenderCallback(&MenuToggles);

#if !XENOMODS_CODENAME(bf3)
			auto gbuffer = section->RegisterSection(std::string(STRINGIFY(RenderingControls)) + "gbuffer", "GBuffer debug...");
			gbuffer->RegisterRenderCallback(&MenuGBuffer);
#endif
		}
	}

	void RenderingControls::Update(fw::UpdateInfo* updateInfo) {
		UpdatableModule::Update(updateInfo);

#if !XENOMODS_CODENAME(bf3)
		if(ForcedParameters.Any()) {
			auto acc = ml::ScnRenderDrSysParmAcc();

			if(ForcedParameters.DisableDOF) {
				acc.setDOFOverride(true);
				acc.setDOF(false);
			}
			if(ForcedParameters.DisableMotionBlur) {
				acc.setMotBlurOverride(true);
				acc.setMotBlur(false);
			}
			if(ForcedParameters.DisableColorFilter) {
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
