#include "plugin.hpp"

#include <cstring>

#include "skyline/logger/Logger.hpp"
//#include "camera.hpp"

namespace bf2mods {

	void Options::Reset() {
		bdatScrambleType = Options::BdatScrambleType::Off;
		//bdatMapListResourceOverride = false;

		disableFallDamage = true;
		movementSpeedMult = 1.f;
	}

	Plugin::Plugin() {
		std::strcpy(m_pluginMeta.name, PLUGIN_NAME);
		m_pluginMeta.version = PLUGIN_VERSION;
		m_pluginMeta.apiVersion = API_VERSION;

		R_ERRORONFAIL(shmemCreate(&m_sharedMemStruct, SHARED_MEM_SIZE, SHARED_MEM_PERM, SHARED_MEM_PERM));
		R_ERRORONFAIL(shmemMap(&m_sharedMemStruct));

		m_sharedState = static_cast<decltype(m_sharedState)>(m_sharedMemStruct.map_addr);
		setDefaultOptions_();
		sharedStateInit_();
	}

	Plugin::~Plugin() = default;

	void Plugin::setDefaultOptions_() {
		// default shared state options
		m_sharedState->options.Reset();

		m_sharedState->freecam.isOn = false;
		m_sharedState->freecam.matrix = mm::Mat44 {};
		m_sharedState->freecam.fov = 45.f;
		m_sharedState->freecam.camSpeed = 1.f;

		m_sharedState->mapjumpId = 1;

		m_sharedState->moonJump = false;
	}

	void Plugin::sharedStateInit_() {
		skyline::logger::s_Instance->LogFormat("Hello World from %s\n", m_pluginMeta.name);

		// anything that needs to be initted based on shared state goes here
	}

} // namespace bf2mods
