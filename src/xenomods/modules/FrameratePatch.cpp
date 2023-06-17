// Created by block on 3/19/23.

#include "FrameratePatch.hpp"

#include "xenomods/engine/ml/DevGraph.hpp"
#include "xenomods/engine/layer/LayerManager.hpp"

namespace {

	// Adapted from a patch by moddingiscool

	struct LayerManagerCtorHook : skylaunch::hook::Trampoline<LayerManagerCtorHook> {
		static void Hook(layer::LayerManager* this_pointer, ml::Scn* scn, mtl::ALLOC_HANDLE handle, unsigned int unk) {
			Orig(this_pointer, scn, handle, unk);
			this_pointer->frameRateFreq = 60.0;
			this_pointer->frameRateTime = 1.0f / this_pointer->frameRateFreq;
			this_pointer->anotherThing = 1.0f / this_pointer->frameRateFreq;
			// prevents DE's UI from running too fast, but still only seems to update at 30...?
			// may also be the source of 3 cutscenes freezing
		}
	};

	struct VSyncHook : skylaunch::hook::Trampoline<VSyncHook> {
		static void Hook(int num) {
			Orig(1);
		}
	};

}

namespace xenomods {

	void FrameratePatch::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up framerate patch...");

		if (GetState().config.enable60FPS) {
#if XENOMODS_CODENAME(bf3)
			LayerManagerCtorHook::HookFromBase(0x710100f260);
			VSyncHook::HookFromBase(0x7101249648);
#else
			LayerManagerCtorHook::HookAt("_ZN5layer12LayerManagerC1EPN2ml3ScnEN3mtl12ALLOC_HANDLEEj"); // can't hook ctors yet?
			VSyncHook::HookAt(&ml::DevGraph::setVSync);
#endif
		}

		// this only works well in DE and 3 right now as they properly have frame skipping
		// 2/Torna do not have it, so the game becomes a slow mess when it can't hit 60 (as is usually the case)
		// these hooks *work*, but it's still ~30fps in game and cutscenes are borked

		// steps to add frame skipping will go here
	}

#if XENOMODS_NEW_ENGINE
	XENOMODS_REGISTER_MODULE(FrameratePatch);
#endif

} // namespace xenomods