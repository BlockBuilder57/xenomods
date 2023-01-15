#include "state.hpp"

#include "bf2mods/bf2logger.hpp"

namespace bf2mods {

	// a bit yucky, but this way it's in one spot
#define CONFIG_PORT_DEFAULT 6969
#define CONFIG_TITLEEVENTS_DEFAULT { 10001, 10504, 10505, 10506, 10507, 10508, 10509, 10510, 10511, 10512, 10513, 10605, 10607, 10608, 10609 }
#define CONFIG_TITLEEVENTSNEEDSCLEAREDGAME_DEFAULT true
#define CONFIG_DUMP_FILEREADS false
#define CONFIG_USE_FILEDUMPS false

	void Config::Reset() {
		port = CONFIG_PORT_DEFAULT;

		// all chapters, game clear, and NG+ clear
		titleEvents = CONFIG_TITLEEVENTS_DEFAULT;
		titleEventsNeedsClearedGame = CONFIG_TITLEEVENTSNEEDSCLEAREDGAME_DEFAULT;

		dumpFileReads = CONFIG_DUMP_FILEREADS;
		useFileDumps = CONFIG_USE_FILEDUMPS;
	}

	void Config::LoadFromFile() {
		toml::parse_result res = toml::parse_file("sd:/config/bf2mods/bf2mods.toml");

		if(!res) {
			auto error = std::move(res).error();
			g_Logger->LogError("Couldn't load config! ({}) Falling back to defaults.", error.description());
			return Reset();
		}

		// load things
		tomlTable = std::move(res).table();

		port = tomlTable["port"].value_or(CONFIG_PORT_DEFAULT);

		if(toml::array* events = tomlTable["titleEvents"].as_array()) {
			bool event_load_failed = false;
			titleEvents.clear();

			events->for_each([&](auto& el) {
				if constexpr(toml::is_integer<decltype(el)>)
					titleEvents.push_back(static_cast<std::uint16_t>(el.get()));
				else {
					event_load_failed = true;
					return false;
				}
			});

			if(event_load_failed)
				titleEvents = CONFIG_TITLEEVENTS_DEFAULT;
		}

		titleEventsNeedsClearedGame = tomlTable["titleEventsNeedsClearedGame"].value_or(CONFIG_TITLEEVENTSNEEDSCLEAREDGAME_DEFAULT);

		dumpFileReads = tomlTable["dumpFileReads"].value_or(CONFIG_DUMP_FILEREADS);
		useFileDumps = tomlTable["useFileDumps"].value_or(CONFIG_DUMP_FILEREADS);

		g_Logger->LogInfo("Loaded config successfully");
	}

	Bf2ModsState& GetState() {
		static Bf2ModsState state;
		return state;
	}

} // namespace bf2mods
