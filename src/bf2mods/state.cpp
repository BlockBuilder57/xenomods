#include "state.hpp"

#include <cstring>

#include "skyline/logger/Logger.hpp"
//#include "camera.hpp"

namespace bf2mods {

	void Options::Reset() {
		bdatScrambleType = Options::BdatScrambleType::Off;

		disableFallDamage = true;
		movementSpeedMult = 1.f;
		enableUIRendering = true;
	}

	Bf2ModsState::Bf2ModsState() {
		Reset();
	}

	void Bf2ModsState::Reset() {
		// Reset options
		options.Reset();

		freecam.isOn = false;
		freecam.matrix = mm::Mat44 {};
		freecam.fov = 45.f;
		freecam.camSpeed = 1.f;

		mapjumpId = 1;

		moonJump = false;
	}

	Bf2ModsState& GetState() {
		static Bf2ModsState state;
		return state;
	}


} // namespace bf2mods
