#pragma once

#include <toml++/toml.h>

#include <cstdint>
#include <vector>

namespace bf2mods {

	struct Config {
		/**
		 * Reset to default values.
		 */
		void Reset();

		void LoadFromFile();

		uint16_t port {};

		std::vector<uint16_t> titleEvents {};
		bool titleEventsNeedsClearedGame {};

		bool dumpFileReads {};
		bool useFileDumps {};
	   private:
		toml::table tomlTable;
	};

	struct Bf2ModsState {
		explicit Bf2ModsState() {
			Reset();

			config.LoadFromFile();
		}

		/**
		 * Reset to clean state.
		 */
		void Reset() {
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
