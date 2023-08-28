#pragma once

#include <toml++/toml.h>

#include <cstdint>
#include <vector>

#include "xenomods/Logger.hpp"
#include "xenomods/Utils.hpp"

namespace xenomods {

	struct Config {
		// very yucky, but this way it's in one spot
#define CONFIG_PORT_DEFAULT 6969
#define CONFIG_LOGGING_LEVEL_DEFAULT underlying_value(Logger::Severity::Info)
#define CONFIG_DUMP_FILE_READS_DEFAULT false
#define CONFIG_ENABLE_FILE_OVERRIDES_DEFAULT true
#define CONFIG_BDAT_SKIP_OVERRIDES_DEFAULT \
	{ "CHR_Ir", "FLD_Condition", "FLD_Quest", "BTL_Enhance", "BTL_Skill_Dr" }
#define CONFIG_DAMAGE_PLAYER_MULT_DEFAULT 1.0
#define CONFIG_DAMAGE_ENEMY_MULT_DEFAULT 1.0
#define CONFIG_ENABLE_60FPS_DEFAULT false
#define CONFIG_DISABLE_BATTLE_BGM_CHANGES false
#if XENOMODS_CODENAME(bf2)
	#define CONFIG_TITLEEVENTS_DEFAULT \
		{ 10001, 10504, 10505, 10506, 10507, 10508, 10509, 10510, 10511, 10512, 10513, 10605, 10607, 10608, 10609 } // all chapters, game clear, and NG+ clear
#else
	#define CONFIG_TITLEEVENTS_DEFAULT \
		{}
#endif
#define CONFIG_EVENT_DEBUG_BITS_DEFAULT 0b1
#define CONFIG_MOUNT_TORNA_CONTENT_DEFAULT false
#define CONFIG_LOAD_FC_LATEST_DEFAULT false

		/**
		 * Reset to default values.
		 */
		void Reset();

		void LoadFromFile();

		uint16_t port {};
		Logger::Severity loggingLevel {};

		bool dumpFileReads {};
		bool enableFileOverrides {};
		std::vector<std::string> bdatSkipOverrides {};

		float damagePlayerMult {};
		float damageEnemyMult {};

		bool enable60FPS {};

		bool disableBattleBGMChanges {};

		// 2/Torna exclusive
		std::vector<uint16_t> titleEvents {};
		unsigned int eventDebugBits {};
		bool mountTornaContent {};

		// DE exclusive
		bool loadFcLatest {};

	   private:
		void InitializeFromTable(const toml::table& table, bool respectDefaults);

		toml::table tomlTable;
	};

	struct XenomodsState;

	/**
	 * Get singleton state.
	 */
	XenomodsState& GetState();

	struct XenomodsState {
		explicit XenomodsState() {
			Reset();

			config.LoadFromFile();
		}

		/**
		 * Reset to clean state.
		 */
		void Reset() {
			config.Reset();
		}

		static void ReloadConfig() {
			GetState().config.LoadFromFile();
		}

		Config config {};
	};

} // namespace xenomods
