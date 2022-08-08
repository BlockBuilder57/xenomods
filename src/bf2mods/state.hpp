#pragma once

#include "bf2mods/utils.hpp"

namespace bf2mods {

	struct Config {
		/**
		 * Reset to default values.
		 */
		constexpr void Reset() {
			port = 6969;

			// all chapters, game clear, and NG+ clear
			titleEvents = { 10001, 10504, 10505, 10506, 10507, 10508, 10509, 10510, 10511, 10512, 10513, 10605, 10607, 10608, 10609 };
			titleEventsNeedsClearedGame = true;
		}

		uint16_t port {};
		std::vector<uint16_t> titleEvents {};
		bool titleEventsNeedsClearedGame {};
	};

	struct Bf2ModsState {
		constexpr explicit Bf2ModsState() {
			Reset();
		}

		/**
		 * Reset to clean state.
		 */
		constexpr void Reset() {
			config.Reset();

			tempInt = 1;
		}

		Config config {};

		int tempInt {};
	};

	/**
	 * Get singleton state.
	 */
	Bf2ModsState& GetState();

} // namespace bf2mods
