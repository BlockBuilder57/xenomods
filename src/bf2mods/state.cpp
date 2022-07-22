#include "state.hpp"

#include <cstring>

#include "skyline/logger/Logger.hpp"
//#include "camera.hpp"

namespace bf2mods {

	Bf2ModsState& GetState() {
		static constinit Bf2ModsState state;
		return state;
	}

} // namespace bf2mods
