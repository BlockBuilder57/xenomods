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

		mp_sharedState = static_cast<decltype(mp_sharedState)>(m_sharedMemStruct.map_addr);
		setDefaultOptions_();
		sharedStateInit_();
	}

	Plugin::~Plugin() {
	}

	void Plugin::setDefaultOptions_() {
		//  mp_sharedState->options.disableVisionBgm = true;
	}

	void Plugin::sharedStateInit_() {
		skyline::logger::s_Instance->LogFormat("Hello World from %s\n", m_pluginMeta.name);
	}

} // namespace bf2mods
