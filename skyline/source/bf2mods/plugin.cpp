#include "plugin.hpp"

#include <string.h>

#include "skyline/logger/Logger.hpp"
//#include "camera.hpp"

namespace bf2mods {

	Plugin::Plugin() {
		strcpy(m_pluginMeta.name, PLUGIN_NAME);
		m_pluginMeta.version = PLUGIN_VERSION;
		m_pluginMeta.apiVersion = API_VERSION;

		R_ERRORONFAIL(shmemCreate(&m_sharedMemStruct, SHARED_MEM_SIZE, SHARED_MEM_PERM, SHARED_MEM_PERM));
		R_ERRORONFAIL(shmemMap(&m_sharedMemStruct));

		m_sharedState = static_cast<decltype(m_sharedState)>(m_sharedMemStruct.map_addr);
		setDefaultOptions_();
		sharedStateInit_();
	}

	Plugin::~Plugin() {
	}

	void Plugin::setDefaultOptions_() {
		// default shared state options
		m_sharedState->options.bdat.scrambleType = SharedState::Options::BdatOptions::ScrambleType::Off;
		m_sharedState->options.bdat.mapListResourceOverride = false;

		m_sharedState->options.game.disableFallDamage = false;
		m_sharedState->options.game.movementSpeedMult = 1.f;

		m_sharedState->mapjumpId = 1;
		m_sharedState->testInt = 1;

		m_sharedState->moonJump = false;
	}

	void Plugin::sharedStateInit_() {
		skyline::logger::s_Instance->LogFormat("Hello World from %s\n", m_pluginMeta.name);

		// anything that needs to be initted based on shared state goes here
	}

} // namespace bf2mods
