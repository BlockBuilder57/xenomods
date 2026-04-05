#include "RenderingControls.hpp"

#include "DebugStuff.hpp"
#include "xenomods/engine/effect/SystemManager.hpp"
#include "xenomods/engine/fw/Managers.hpp"
#include "xenomods/engine/gf/MenuObject.hpp"
#include "xenomods/engine/grlib/CGLibDisplay.hpp"
#include "xenomods/engine/grlib/CGLibRenderTarget.hpp"
#include "xenomods/engine/layer/LayerManager.hpp"
#include "xenomods/engine/layer/LayerObj.hpp"
#include "xenomods/engine/mm/mtl/PtrSingleton.hpp"
#include "xenomods/engine/ptlib/Emitter.hpp"
#include "xenomods/engine/ui/UIObjectAcc.hpp"
#include "xenomods/engine/xefb/Effect.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"

#define QOI_IMPLEMENTATION
#include <qoi.h>

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

	struct SkipFogRendering : skylaunch::hook::Trampoline<SkipFogRendering> {
		static bool Hook(ml::DrFogMan* this_pointer) {
			return xenomods::RenderingControls::skipFogRendering ? false : Orig(this_pointer);
		}
	};

	struct SkipDepthOfFieldRendering : skylaunch::hook::Trampoline<SkipDepthOfFieldRendering> {
		static void Hook(void* this_pointer) {
			if(!xenomods::RenderingControls::skipDepthOfFieldRendering)
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

	struct DisableFadeMode : skylaunch::hook::Trampoline<DisableFadeMode> {
		static void Hook(gf::GfObjAcc* objAcc, bool param) {
			if (!xenomods::RenderingControls::disableModelFade)
				return Orig(objAcc, param);
			Orig(objAcc, false);
		}
	};

	struct DisableFadeAlpha : skylaunch::hook::Trampoline<DisableFadeAlpha> {
		static bool Hook(gf::GfObjAcc* objAcc) {
			if (!xenomods::RenderingControls::disableModelFade)
				return Orig(objAcc);
			return false;
		}
	};
#endif

#if XENOMODS_CODENAME(bfsw)
	// hooking ml::DrMdlObj::setCallSpAlphaMode makes parts of a model fade out independently
	struct DisableFade1DE : skylaunch::hook::Trampoline<DisableFade1DE> {
		static void Hook(ml::DrMdlObj* mdlObj, bool param) {
			if (!xenomods::RenderingControls::disableModelFade)
				return Orig(mdlObj, param);
			Orig(mdlObj, false);
		}
	};
#endif

	int SWIZZLE_LOOKUP[] = {
	     0,  4,  1,  5,
	     8, 12,  9, 13,
	    16, 20, 17, 21,
	    24, 28, 25, 29,
	     2,  6,  3,  7,
	    10, 14, 11, 15,
	    18, 22, 19, 23,
	    26, 30, 27, 31
	};

	const int TILE_WIDTH = 4;
	const int TILE_HEIGHT = 8;

	// adapted from XBC2ModelDecomp! jeez
	void deswizzle(void* swizBuf, void* deswizBuf, int width, int height) {
		if (!swizBuf || !deswizBuf) {
			xenomods::g_Logger->LogError("Couldn't deswizzle because a buffer was missing!");
			return;
		}

	    const int bytesPerPixel = 4;
	    const int dataSize = width * height * bytesPerPixel;

	    // these aren't really "row" or "column". I don't know what these are actually called
	    // for the resolutions/types this uses, these are constant
	    const int tileRowCount = 16;
	    const int tileColumnCount = 4;

	    // these loops are in proper order! don't touch them!

	    int dataOrigIdx = 0;
	    for (int tileY = 0; tileY < (height / TILE_HEIGHT) / tileRowCount; tileY++) {
	        for (int tileX = 0; tileX < (width / TILE_WIDTH) / tileColumnCount; tileX++) {
	            for (int curTileX = 0; curTileX < tileRowCount; curTileX++) {
	                for (int SwizzleIndex = 0; SwizzleIndex < 32; SwizzleIndex++) {
	                    for (int curTileY = 0; curTileY < tileColumnCount; curTileY++) {
	                        int l = SWIZZLE_LOOKUP[SwizzleIndex];

	                        int deswizzledHeight = (tileY * tileRowCount + curTileX) * TILE_HEIGHT + (l / TILE_WIDTH);
	                        int deswizzledWidth = (tileX * bytesPerPixel + (l % TILE_WIDTH)) * tileColumnCount + curTileY;
	                        // bytesPerPixel above may be wrong, was 4

	                        int destIdx = bytesPerPixel * (deswizzledHeight * width + deswizzledWidth);

	                    	if (destIdx > dataSize || dataOrigIdx > dataSize) {
	                    		xenomods::g_Logger->LogError("Tried to deswizzle out of bounds, something's wrong");
	                    		return;
	                    	}

	                    	memcpy(deswizBuf + destIdx, swizBuf + dataOrigIdx, bytesPerPixel);
	                        dataOrigIdx += bytesPerPixel;
	                    }
	                }
	            }
	        }
	    }
	}

#if !XENOMODS_CODENAME(bf3)
	struct CustomResolution : skylaunch::hook::Trampoline<CustomResolution> {
		static void Hook(void* this_pointer, int w, int h) {
			Orig(this_pointer, xenomods::GetState().config.customRes[0], xenomods::GetState().config.customRes[1]);
		}
	};

	struct FramebufferHook : skylaunch::hook::Trampoline<FramebufferHook> {
		static void Hook(grlib::CGLibDisplay* this_pointer, void* NVNqueue) {
			Orig(this_pointer, NVNqueue);

			auto& capParam = xenomods::RenderingControls::CapParameters;

			if (capParam.WaitFrames == 0) {
				std::size_t storageSize = this_pointer->rtRender0.textureBuf.storageSize;
				void* storageBuf = this_pointer->rtRender0.textureBuf.storageBuffer;

				//xenomods::g_Logger->LogDebug("0x{:x} at {}, format {:x}", storageSize, this_pointer->rtRender0.textureBuf.storageBuffer, this_pointer->rtRender0.textureBuf.grlSurfaceFormat);
				//dbgutil::dumpMemory(this_pointer->rtRender0.textureBuf.storageBuffer, storageSize, "renderTarget0.dump");

				// we have to do the actual dumping here

				std::string filename = capParam.PathSuffix;
				std::string path = "";
				{
					bool shouldMakeTimestamp = filename.empty() || filename.find_first_of("{}") != 0;

					if (shouldMakeTimestamp) {
						// see MenuLog::SaveToFile for notes about this timestamp nonsense
						nn::time::PosixTime time {};
						nn::time::StandardUserSystemClock::GetCurrentTime(&time);
						std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> tp { std::chrono::seconds { time.time } };
						std::string timestamp = std::format("{:%F-%H-%M-%S}", tp);

						xenomods::StringReplace(filename, "{}", timestamp);
					}

					// append extension
					switch (xenomods::GetState().config.captureFormat) {
						case 0:
						default:
							filename += ".data";
							break;
						case 1:
							filename += ".qoi";
							break;
					}

					path = XENOMODS_CONFIG_PATH "/screenshots/" + filename;
				}

				void* deswizBuf = static_cast<char*>(malloc(storageSize));
				memset(deswizBuf, 0, storageSize);

				unsigned int* res = &xenomods::GetState().config.customRes[0];

				deswizzle(storageBuf, deswizBuf, res[0], res[1]);

				// (imgui_xeno? ImGui?) writes alpha values, we don't want those
				for (int i = 0; i < storageSize; i += 4) {
					static_cast<uint8_t*>(deswizBuf)[i+3] = 255;
				}

				qoi_desc qoiDesc = {
					.width = res[0],
					.height = res[1],
					.channels = 4,
					.colorspace = QOI_SRGB
				};
				int qoiSize = 0;
				void* qoiBuf = qoi_encode(deswizBuf, &qoiDesc, &qoiSize);

				free(deswizBuf);

				if(!xenomods::NnFile::Preallocate(path, qoiSize)) {
					xenomods::g_Logger->LogError("Couldn't create/preallocate screenshot \"{}\"", path);
					return;
				}

				xenomods::NnFile file(path, nn::fs::OpenMode_Write);

				if(!file) {
					xenomods::g_Logger->LogError("Couldn't open screenshot \"{}\"", path);
				} else {
					file.Write(qoiBuf, qoiSize);
				}
				file.Flush();
				file.Close();

				free(qoiBuf);

				xenomods::g_Logger->LogDebug("Saved screenshot as \"{}\"", filename);

				// re-open menu if it was closed
				if (xenomods::RenderingControls::CapParameters.WasMenuOpen && !xenomods::g_Menu->IsOpen())
					xenomods::g_Menu->Toggle();
				if (xenomods::RenderingControls::CapParameters.WasUIEnabled && xenomods::RenderingControls::skipUIRendering)
					xenomods::RenderingControls::skipUIRendering = false;

				xenomods::RenderingControls::CapParameters.Reset();
			}
			else {
				xenomods::RenderingControls::CapParameters.WaitFrames--;
				if (xenomods::RenderingControls::CapParameters.WaitFrames < -1)
					xenomods::RenderingControls::CapParameters.WaitFrames = -1;
			}
		}
	};
	#endif

} // namespace

namespace xenomods {

	RenderingControls::ForcedRenderParameters RenderingControls::ForcedParameters = {};
	RenderingControls::CaptureParameters RenderingControls::CapParameters = {};

	bool RenderingControls::straightenFont = false;
	bool RenderingControls::skipUIRendering = false;
	bool RenderingControls::skipParticleRendering = false;
	bool RenderingControls::skipOverlayRendering = false;
	bool RenderingControls::skipCloudRendering = false;
	bool RenderingControls::skipSkyDomeRendering = false;
	bool RenderingControls::skipFogRendering = false;
	bool RenderingControls::skipDepthOfFieldRendering = false;
	bool RenderingControls::enableAutoReduction = true;
	bool RenderingControls::disableModelFade = false;

	float RenderingControls::shadowStrength = 1.0;

	bool RenderingControls::freezeTextureStreaming = false;

	const std::string toggleKey = std::string(STRINGIFY(RenderingControls)) + "_Toggles";

	void RenderingControls::QueueScreenshot(std::string suffix /*= ""*/, int wait_frames /*= 2*/) {
		CapParameters.PathSuffix = suffix;
		CapParameters.WaitFrames = 2;
		CapParameters.WasMenuOpen = g_Menu->IsOpen();
		CapParameters.WasUIEnabled = !skipUIRendering;

		// hide menu
		if (CapParameters.WasMenuOpen)
			g_Menu->Toggle();
		// disable ui rendering
		skipUIRendering = true;
	}

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

		if (ImGui::Button("Queue screenshot"))
			QueueScreenshot();
		ImGui::SameLine();
		ImGui::Checkbox("Disable UI while capturing", &CapParameters.ShouldDisableUI);
		if (xenomods::version::BuildIsDebug && CapParameters.WaitFrames > 0) {
			ImGui::Text("%d frames to cap", CapParameters.WaitFrames);
		}
	}

	void RenderingControls::MenuToggles() {
		ImGui::Checkbox("Skip UI rendering", &skipUIRendering);
		ImGui::Checkbox("Skip cloud (sea) rendering", &skipCloudRendering);
		ImGui::Checkbox("Skip sky dome rendering", &skipSkyDomeRendering);
		ImGui::Checkbox("Skip fog rendering", &skipFogRendering);
		ImGui::Checkbox("Skip depth of field rendering", &skipDepthOfFieldRendering);
#if !XENOMODS_CODENAME(bf3)
		ImGui::Checkbox("Disable model fading", &disableModelFade);
	#if XENOMODS_OLD_ENGINE
		ImGui::Checkbox("Skip particle+overlay rendering", &skipParticleRendering);
	#else
		ImGui::Checkbox("Skip particle rendering", &skipParticleRendering);
		ImGui::Checkbox("Skip overlay rendering", &skipOverlayRendering);
	#endif

		auto acc = ml::ScnRenderDrSysParmAcc();
		static bool hideBloom = false, hideTonemap = false;

		ImGui::Checkbox("Hide map", &acc.drMan->hideMap);
		if(ImGui::Checkbox("Disable bloom", &hideBloom))
			acc.setBloom(!hideBloom);
		if(ImGui::Checkbox("Disable tonemapping", &hideTonemap))
			acc.setToneMap(!hideTonemap);

		ImGui::Checkbox("Force disable motion blur", &ForcedParameters.DisableMotionBlur);
		ImGui::Checkbox("Force disable color filters", &ForcedParameters.DisableColorFilter);
		ImGui::Checkbox("Enable AA", &acc.PixlPostParm->enableAA);
		ImGui::Checkbox("Enable TMAA", &acc.PixlPostParm->enableTMAA);
		if(ImGui::Checkbox("Enable resolution scaling", &enableAutoReduction))
			acc.setAutoReduction(enableAutoReduction);
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
		SkipSkyDomeRendering::HookAt("_ZN2ml13DrPixlSkyDome10baseRenderEv");
		SkipDepthOfFieldRendering::HookAt("_ZN2ml25DrPixlPostObjDepthofField10renderCalcEv");
		SkipFogRendering::HookAt("_ZNK2ml8DrFogMan7isFogOnEv");

		FreezeTextureStreaming::HookAt(&ml::DrResMdoTexList::texStmUpdate);
		CustomResolution::HookAt(&grlib::CGLibDisplay::resize);
		FramebufferHook::HookAt("_ZN5grlib12CGLibDisplay14updateSignaledEv");
#else
		// all here match up to the symbols up above
		// Layer2 is an unnamed function immediately next to the normal finalRender.
		if (version::RuntimeVersion() == version::SemVer::v2_0_0) {
			SkipLayerRendering::HookFromBase(0x710100f790);
			SkipLayer2Rendering::HookFromBase(0x710100f808);
			SkipCloudRendering::HookFromBase(0x7100ec3598);
			SkipSkyDomeRendering::HookFromBase(0x7100f6a9e8);
			SkipDepthOfFieldRendering::HookFromBase(0x7100f6e668);
			SkipFogRendering::HookFromBase(0x7100ef5d70);
		}
		else if (version::RuntimeVersion() == version::SemVer::v2_1_0) {
			SkipLayerRendering::HookFromBase(0x710100fac0);
			SkipLayer2Rendering::HookFromBase(0x710100fb38);
			SkipCloudRendering::HookFromBase(0x7100ec38c8);
			SkipSkyDomeRendering::HookFromBase(0x7100f6ad18);
			SkipDepthOfFieldRendering::HookFromBase(0x7100f6e998);
			SkipFogRendering::HookFromBase(0x7100ef60a0);
		}
		else if (version::RuntimeVersion() == version::SemVer::v2_1_1) {
			SkipLayerRendering::HookFromBase(0x710100fb00);
			SkipLayer2Rendering::HookFromBase(0x710100fb78);
			SkipCloudRendering::HookFromBase(0x7100ec3908);
			SkipSkyDomeRendering::HookFromBase(0x7100f6ad58);
			SkipDepthOfFieldRendering::HookFromBase(0x7100f6e9d8);
			SkipFogRendering::HookFromBase(0x7100ef60e0);
		}
		else if (version::RuntimeVersion() == version::SemVer::v2_2_0) {
			SkipLayerRendering::HookFromBase(0x7101010670);
			SkipLayer2Rendering::HookFromBase(0x71010106e8);
			SkipCloudRendering::HookFromBase(0x7100ec4478);
			SkipSkyDomeRendering::HookFromBase(0x7100f6b8c8);
			SkipDepthOfFieldRendering::HookFromBase(0x7100f6f548);
			SkipFogRendering::HookFromBase(0x7100ef6c50);
		}
#endif

#if XENOMODS_OLD_ENGINE
		StraightensYourXenoblade::HookAt("_ZN5layer12LayerObjFont17updateShaderParmsEPKNS_15LayerRenderViewERKNS_14LayerResMatrixERKNS_13LayerResColorE");
		SkipParticleRendering::HookAt("_ZN5ptlib15ParticleManager4drawEPKNS_16DrDrawWorkInfoEFEib");
		DisableFadeMode::HookAt("_ZN2gf8GfObjAcc19setRenderCameraFadeEb");
		DisableFadeAlpha::HookAt("_ZNK2gf8GfObjAcc17isCameraFadeAllOKEv");
#elif XENOMODS_CODENAME(bfsw)
		SkipParticleRendering::HookAt("_ZN4xefb14CEParticlelist4drawEPNS_5CEResE");
		DisableFade1DE::HookAt("_ZN2ml8DrMdlObj18setCallSpAlphaModeEb");
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
