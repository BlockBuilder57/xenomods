#pragma once

#include "bf2mods/fw/camera.hpp"
#include "utils.hpp"

#ifdef __SKYLINE__
	#include "nn/hid.hpp"
#else
	#include <switch.h>
#endif

namespace bf2mods {

#ifdef __SKYLINE__
	using HidControllerKeyData = nn::hid::HidControllerKeys;
#else
	using HidControllerKeyData = HidControllerKeys;
#endif

	STATIC_ASSERT_SIZE(HidControllerKeyData, 4);

	struct SharedState {
		struct Options {
			bool disableVisionBgm;

			struct FreeCamOptions {
				fw::DEBUG_CAMERA freeCamType;
				struct KeyMap {
					HidControllerKeyData toggle;
					HidControllerKeyData forward;
					HidControllerKeyData backward;
					HidControllerKeyData zoomIn;
					HidControllerKeyData zoomOut;
					HidControllerKeyData fovIncrease;
					HidControllerKeyData fovDecrease;
					HidControllerKeyData bankLeft;
					HidControllerKeyData bankRight;
					HidControllerKeyData teleport;
				} keyMap;
			} freeCam;

		} options;

		struct FreeCamState {
			bool isToggledOn;
		} freeCam;
	};

#define ALIGN_UP(x, a) (((x) + ((a)-1)) & ~((a)-1))
	static constexpr auto SHARED_MEM_SIZE = ALIGN_UP(sizeof(SharedState), 0x1000);

} // namespace ether
