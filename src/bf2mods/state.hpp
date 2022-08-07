#pragma once

#include "bf2mods/utils.hpp"

namespace bf2mods {

	struct Bf2ModsState {
		constexpr explicit Bf2ModsState() {
			Reset();
		}

		/**
		 * Reset to clean state.
		 */
		constexpr void Reset() {
			tempInt = 1;
		}

		int tempInt {};
	};

	/**
	 * Get singleton state.
	 */
	Bf2ModsState& GetState();

} // namespace bf2mods

