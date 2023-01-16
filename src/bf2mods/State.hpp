#pragma once

#include <toml++/toml.h>

#include <cstdint>
#include <vector>

namespace bf2mods {

	struct Config {

		// very yucky, but this way it's in one spot
#define CONFIG_PORT_DEFAULT 6969
#define CONFIG_TITLEEVENTS_DEFAULT { 10001, 10504, 10505, 10506, 10507, 10508, 10509, 10510, 10511, 10512, 10513, 10605, 10607, 10608, 10609 }
#define CONFIG_DUMP_FILEREADS false
#define CONFIG_USE_FILEDUMPS false

		/**
		 * Reset to default values.
		 */
		void Reset();

		void LoadFromFile();

		uint16_t port {};

		std::vector<uint16_t> titleEvents {};

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
