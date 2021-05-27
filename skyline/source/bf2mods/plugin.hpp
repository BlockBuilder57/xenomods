#pragma once

#include "bf2mods/shared_state.hpp"
#include "skyline/efl/service.hpp"
#include "skyline/nx/kernel/shmem.h"

namespace bf2mods {

	class Plugin {
	   private:
		Plugin();
		Plugin(const Plugin&) = delete;
		~Plugin();

		static inline Plugin& getInstance() {
			static Plugin s_instance;
			return s_instance;
		}

		static constexpr auto SHARED_MEM_PERM = Perm_Rw;

		SlPluginMeta m_pluginMeta;

		SharedMemory m_sharedMemStruct;
		SharedState* mp_sharedState;

		void setDefaultOptions_();
		void sharedStateInit_();

		inline SlPluginMeta& getPluginMeta_() {
			return m_pluginMeta;
		}

		inline SlPluginSharedMemInfo getPluginSharedMemInfo_() const {
			auto sharedMemStruct = m_sharedMemStruct;
			return SlPluginSharedMemInfo { sharedMemStruct.handle, sharedMemStruct.size, sharedMemStruct.perm };
		}

		inline bf2mods::SharedState* getSharedStatePtr_() const {
			return mp_sharedState;
		}

	   public:
		static constexpr auto PLUGIN_NAME = "bf2mods";
		static constexpr auto PLUGIN_VERSION = 0;
		static constexpr auto API_VERSION = 0;

		static inline void init() {
			getInstance();
		}

		static inline SlPluginMeta& getPluginMeta() {
			return getInstance().getPluginMeta_();
		}

		static inline SlPluginSharedMemInfo getPluginSharedMemInfo() {
			return getInstance().getPluginSharedMemInfo_();
		}

		static inline bf2mods::SharedState* getSharedStatePtr() {
			return getInstance().getSharedStatePtr_();
		}
	};

} // namespace bf2mods
