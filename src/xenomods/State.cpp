#include <xenomods/State.hpp>
#include <xenomods/Logger.hpp>

#include "modules/UpdatableModule.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"

namespace xenomods {

	void Config::Reset() {
		port = CONFIG_PORT_DEFAULT;
		loggingLevel = static_cast<Logger::Severity>(CONFIG_LOGGING_LEVEL_DEFAULT);

		menuTheme = static_cast<Menu::Theme>(CONFIG_MENU_THEME_DEFAULT);
		menuFonts = CONFIG_MENU_FONTS_DEFAULT;

		titleEvents = CONFIG_TITLEEVENTS_DEFAULT;
		if (version::RuntimeGame() == version::GameType::BF2)
			titleEvents = CONFIG_TITLEEVENTS_DEFAULT_BF2;

		eventDebugBits = CONFIG_EVENT_DEBUG_BITS_DEFAULT;

		dumpFileReads = CONFIG_DUMP_FILE_READS_DEFAULT;
		enableFileOverrides = CONFIG_ENABLE_FILE_OVERRIDES_DEFAULT;
		bdatSkipOverrides = CONFIG_BDAT_SKIP_OVERRIDES_DEFAULT;

		damagePlayerMult = CONFIG_DAMAGE_PLAYER_MULT_DEFAULT;
		damageEnemyMult = CONFIG_DAMAGE_ENEMY_MULT_DEFAULT;

		disableBattleBGMChanges = CONFIG_DISABLE_BATTLE_BGM_CHANGES;

		enable60FPS = CONFIG_ENABLE_60FPS_DEFAULT;

		mountTornaContent = CONFIG_MOUNT_TORNA_CONTENT_DEFAULT;

		loadFcLatest = CONFIG_LOAD_FC_LATEST_DEFAULT;
	}

	void Config::LoadFromFile() {
		toml::parse_result res = toml::parse_file(XENOMODS_CONFIG_PATH "/config.toml");

		if(!res) {
			auto error = std::move(res).error();
			if (error.description() == "File could not be opened for reading")
				g_Logger->LogWarning("Config file was not found. Falling back to defaults.");
			else
				g_Logger->LogWarning("Couldn't load config! ({}) Falling back to defaults.", error.description());
			return Reset();
		}

		// load things
		tomlTable = std::move(res).table();

		InitializeFromTable(tomlTable, true);

		// update the logger's level
		g_Logger->SetLoggingLevel(loggingLevel);
		// update menu
		g_Menu->SetTheme(menuTheme);

		// update modules
		ConfigUpdateForAllRegisteredModules();

		g_Logger->ToastInfo("config", "Loaded config!");
	}

	void Config::InitializeFromTable(const toml::table& table, bool respectDefaults) {
		if(respectDefaults || table[STRINGIFY(port)].type() != toml::node_type::none)
			port = table[STRINGIFY(port)].value_or(CONFIG_PORT_DEFAULT);

		if(respectDefaults || table[STRINGIFY(loggingLevel)].type() != toml::node_type::none)
			loggingLevel = static_cast<Logger::Severity>(table[STRINGIFY(loggingLevel)].value_or(CONFIG_LOGGING_LEVEL_DEFAULT));

		if(respectDefaults || table[STRINGIFY(menuTheme)].type() != toml::node_type::none)
			menuTheme = static_cast<Menu::Theme>(table[STRINGIFY(menuTheme)].value_or(CONFIG_MENU_THEME_DEFAULT));
		if(table[STRINGIFY(menuFonts)].is_array()) {
			const toml::array* arr = table[STRINGIFY(menuFonts)].as_array();
			bool load_failed = false;
			menuFonts.clear();

			if(arr != nullptr) {
				arr->for_each([&](auto& el) {
					if constexpr(toml::is_array<decltype(el)>) {
						const toml::array* fontDef = el.as_array();

						if (fontDef != nullptr) {
							std::string path = fontDef->get(0)->value_or("");
							float fontSize = fontDef->get(1)->value_or(0.0f);

							menuFonts[path] = fontSize;
						}
					}
					else {
						load_failed = true;
						return false;
					}
				});
			} else
				load_failed = true;

			if(load_failed && respectDefaults)
				menuFonts = CONFIG_MENU_FONTS_DEFAULT;
		}

		if(respectDefaults || table[STRINGIFY(dumpFileReads)].type() != toml::node_type::none)
			dumpFileReads = table[STRINGIFY(dumpFileReads)].value_or(CONFIG_DUMP_FILE_READS_DEFAULT);
		if(respectDefaults || table[STRINGIFY(enableFileOverrides)].type() != toml::node_type::none)
			enableFileOverrides = table[STRINGIFY(enableFileOverrides)].value_or(CONFIG_ENABLE_FILE_OVERRIDES_DEFAULT);
		if(table[STRINGIFY(bdatSkipOverrides)].is_array()) {
			const toml::array* arr = table[STRINGIFY(bdatSkipOverrides)].as_array();
			bool load_failed = false;
			bdatSkipOverrides.clear();

			if(arr != nullptr) {
				arr->for_each([&](auto& el) {
					if constexpr(toml::is_string<decltype(el)>)
						bdatSkipOverrides.push_back(el.get());
					else {
						load_failed = true;
						return false;
					}
				});
			} else
				load_failed = true;

			if(load_failed && respectDefaults)
				bdatSkipOverrides = CONFIG_BDAT_SKIP_OVERRIDES_DEFAULT;
		}

		if(respectDefaults || table[STRINGIFY(damagePlayerMult)].type() != toml::node_type::none)
			damagePlayerMult = table[STRINGIFY(damagePlayerMult)].value_or(CONFIG_DAMAGE_PLAYER_MULT_DEFAULT);
		if(respectDefaults || table[STRINGIFY(damageEnemyMult)].type() != toml::node_type::none)
			damageEnemyMult = table[STRINGIFY(damageEnemyMult)].value_or(CONFIG_DAMAGE_ENEMY_MULT_DEFAULT);

		if(respectDefaults || table[STRINGIFY(enable60FPS)].type() != toml::node_type::none)
			enable60FPS = table[STRINGIFY(enable60FPS)].value_or(CONFIG_ENABLE_60FPS_DEFAULT);

		if(respectDefaults || table[STRINGIFY(disableBattleBGMChanges)].type() != toml::node_type::none)
			disableBattleBGMChanges = table[STRINGIFY(disableBattleBGMChanges)].value_or(CONFIG_DISABLE_BATTLE_BGM_CHANGES);

		// 2/Torna exclusive
		if(table[STRINGIFY(titleEvents)].is_array()) {
			const toml::array* arr = table[STRINGIFY(titleEvents)].as_array();
			bool load_failed = false;
			titleEvents.clear();

			if(arr != nullptr) {
				arr->for_each([&](auto& el) {
					if constexpr(toml::is_integer<decltype(el)>)
						titleEvents.push_back(static_cast<std::uint16_t>(el.get()));
					else {
						load_failed = true;
						return false;
					}
				});
			} else
				load_failed = true;

			if(load_failed && respectDefaults) {
				titleEvents = CONFIG_TITLEEVENTS_DEFAULT;
				if (version::RuntimeGame() == version::GameType::BF2)
					titleEvents = CONFIG_TITLEEVENTS_DEFAULT_BF2;
			}
		}

		if(respectDefaults || table[STRINGIFY(eventDebugBits)].type() != toml::node_type::none)
			eventDebugBits = table[STRINGIFY(eventDebugBits)].value_or(CONFIG_EVENT_DEBUG_BITS_DEFAULT);

		if(respectDefaults || table[STRINGIFY(mountTornaContent)].type() != toml::node_type::none)
			mountTornaContent = table[STRINGIFY(mountTornaContent)].value_or(CONFIG_MOUNT_TORNA_CONTENT_DEFAULT);

		// DE exclusive
		if(respectDefaults || table[STRINGIFY(loadFcLatest)].type() != toml::node_type::none)
			loadFcLatest = table[STRINGIFY(loadFcLatest)].value_or(CONFIG_LOAD_FC_LATEST_DEFAULT);

		if(respectDefaults && table[XENOMODS_CODENAME_STR].is_table()) {
			//g_Logger->LogDebug("Found {} as a category, loading...", XENOMODS_CODENAME_STR);
			InitializeFromTable(*table[XENOMODS_CODENAME_STR].as_table(), false);
		}
	}

	XenomodsState& GetState() {
		static XenomodsState state;
		return state;
	}

} // namespace xenomods
