#include "State.hpp"

#include <xenomods/Logger.hpp>

#include "modules/UpdatableModule.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"

namespace xenomods {

	void Config::Reset() {
		port = CONFIG_PORT_DEFAULT;

		titleEvents = CONFIG_TITLEEVENTS_DEFAULT;

		eventDebugBits = CONFIG_EVENT_DEBUG_BITS_DEFAULT;

		dumpFileReads = CONFIG_DUMP_FILE_READS_DEFAULT;
		enableFileOverrides = CONFIG_ENABLE_FILE_OVERRIDES_DEFAULT;
		bdatSkipOverrides = CONFIG_BDAT_SKIP_OVERRIDES_DEFAULT;

		mountTornaContent = CONFIG_MOUNT_TORNA_CONTENT_DEFAULT;
	}

	void Config::LoadFromFile() {
		toml::parse_result res = toml::parse_file("sd:/config/xenomods/config.toml");

		if(!res) {
			auto error = std::move(res).error();
			g_Logger->LogWarning("Couldn't load config.toml! ({}) Falling back to defaults.", error.description());
			return Reset();
		}

		// load things
		tomlTable = std::move(res).table();

		InitializeFromTable(tomlTable, true);

		ConfigUpdateForAllRegisteredModules();
		g_Logger->LogInfo("Loaded config.toml successfully!");
	}

	void Config::InitializeFromTable(const toml::table& table, bool respectDefaults) {
		if (respectDefaults || table[STRINGIFY(port)].type() != toml::node_type::none)
			port = table[STRINGIFY(port)].value_or(CONFIG_PORT_DEFAULT);

		if(table[STRINGIFY(titleEvents)].is_array()) {
			const toml::array* arr = table[STRINGIFY(titleEvents)].as_array();
			bool load_failed = false;
			titleEvents.clear();

			if (arr != nullptr) {
				arr->for_each([&](auto& el) {
					if constexpr(toml::is_integer<decltype(el)>)
						titleEvents.push_back(static_cast<std::uint16_t>(el.get()));
					else {
						load_failed = true;
						return false;
					}
				});
			}
			else
				load_failed = true;

			if(load_failed && respectDefaults)
				titleEvents = CONFIG_TITLEEVENTS_DEFAULT;
		}

		if (respectDefaults || table[STRINGIFY(eventDebugBits)].type() != toml::node_type::none)
			eventDebugBits = table[STRINGIFY(eventDebugBits)].value_or(CONFIG_EVENT_DEBUG_BITS_DEFAULT);

		if (respectDefaults || table[STRINGIFY(dumpFileReads)].type() != toml::node_type::none)
			dumpFileReads = table[STRINGIFY(dumpFileReads)].value_or(CONFIG_DUMP_FILE_READS_DEFAULT);
		if (respectDefaults || table[STRINGIFY(enableFileOverrides)].type() != toml::node_type::none)
			enableFileOverrides = table[STRINGIFY(enableFileOverrides)].value_or(CONFIG_ENABLE_FILE_OVERRIDES_DEFAULT);
		if(table[STRINGIFY(bdatSkipOverrides)].is_array()) {
			const toml::array* arr = table[STRINGIFY(bdatSkipOverrides)].as_array();
			bool load_failed = false;
			bdatSkipOverrides.clear();

			if (arr != nullptr) {
				arr->for_each([&](auto& el) {
					if constexpr(toml::is_string<decltype(el)>)
						bdatSkipOverrides.push_back(el.get());
					else {
						load_failed = true;
						return false;
					}
				});
			}
			else
				load_failed = true;

			if(load_failed && respectDefaults)
				bdatSkipOverrides = CONFIG_BDAT_SKIP_OVERRIDES_DEFAULT;
		}

		if (respectDefaults || table[STRINGIFY(mountTornaContent)].type() != toml::node_type::none)
			mountTornaContent = table[STRINGIFY(mountTornaContent)].value_or(CONFIG_MOUNT_TORNA_CONTENT_DEFAULT);

		if (respectDefaults && table[XENOMODS_CODENAME_STR].is_table()) {
			//g_Logger->LogDebug("Found {} as a category, loading...", XENOMODS_CODENAME_STR);
			InitializeFromTable(*table[XENOMODS_CODENAME_STR].as_table(), false);
		}
	}

	XenomodsState& GetState() {
		static XenomodsState state;
		return state;
	}

} // namespace xenomods
