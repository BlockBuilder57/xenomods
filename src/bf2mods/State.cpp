#include "State.hpp"

#include <bf2mods/stuff/utils/debug_util.hpp>

#include "bf2mods/Logger.hpp"

namespace bf2mods {

	void Config::Reset() {
		port = CONFIG_PORT_DEFAULT;

		titleEvents = CONFIG_TITLEEVENTS_DEFAULT;

		eventDebugBits = CONFIG_EVENT_DEBUG_BITS_DEFAULT;

		dumpFileReads = CONFIG_DUMP_FILEREADS_DEFAULT;
		useFileDumps = CONFIG_USE_FILEDUMPS_DEFAULT;
	}

	void Config::LoadFromFile() {
		toml::parse_result res = toml::parse_file("sd:/config/bf2mods/bf2mods.toml");

		if(!res) {
			auto error = std::move(res).error();
			g_Logger->LogWarning("Couldn't load config! ({}) Falling back to defaults.", error.description());
			return Reset();
		}

		// load things
		tomlTable = std::move(res).table();

		InitializeFromTable(tomlTable, true);

		g_Logger->LogInfo("Loaded config successfully!");
	}

	void Config::InitializeFromTable(const toml::table& table, bool respectDefaults) {
		if (respectDefaults || table["port"].type() != toml::node_type::none)
			port = table["port"].value_or(CONFIG_PORT_DEFAULT);

		if(table["titleEvents"].is_array()) {
			const toml::array* events = table["titleEvents"].as_array();
			bool event_load_failed = false;
			titleEvents.clear();

			if (events != nullptr) {
				events->for_each([&](auto& el) {
					if constexpr(toml::is_integer<decltype(el)>)
						titleEvents.push_back(static_cast<std::uint16_t>(el.get()));
					else {
						event_load_failed = true;
						return false;
					}
				});
			}
			else
				event_load_failed = true;

			if(event_load_failed && respectDefaults)
				titleEvents = CONFIG_TITLEEVENTS_DEFAULT;
		}

		if (respectDefaults || table["eventDebugBits"].type() != toml::node_type::none)
			eventDebugBits = table["eventDebugBits"].value_or(CONFIG_EVENT_DEBUG_BITS_DEFAULT);

		if (respectDefaults || table["dumpFileReads"].type() != toml::node_type::none)
			dumpFileReads = table["dumpFileReads"].value_or(CONFIG_DUMP_FILEREADS_DEFAULT);
		if (respectDefaults || table["useFileDumps"].type() != toml::node_type::none)
			useFileDumps = table["useFileDumps"].value_or(CONFIG_USE_FILEDUMPS_DEFAULT);

		if (respectDefaults && table[BF2MODS_CODENAME_STR].is_table()) {
			//g_Logger->LogDebug("Found {} as a category, loading...", BF2MODS_CODENAME_STR);
			InitializeFromTable(*table[BF2MODS_CODENAME_STR].as_table(), false);
		}
	}

	Bf2ModsState& GetState() {
		static Bf2ModsState state;
		return state;
	}

} // namespace bf2mods
