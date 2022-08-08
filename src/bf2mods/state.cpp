#include "state.hpp"

namespace bf2mods {

	Bf2ModsState& GetState() {
		static Bf2ModsState state;
		return state;
	}

} // namespace bf2mods
