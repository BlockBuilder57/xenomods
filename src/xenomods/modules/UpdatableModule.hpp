#pragma once

#include <type_traits>
#include <imgui.h>
#include <imgui_xeno.h>
#include <misc/cpp/imgui_stdlib.h>

#include <skylaunch/hookng/Hooks.hpp>
#include <xenomods/DebugWrappers.hpp>
#include <xenomods/HidInput.hpp>
#include <xenomods/Logger.hpp>
#include <xenomods/NnFile.hpp>
#include <xenomods/State.hpp>
#include <xenomods/Utils.hpp>
#include <xenomods/Version.hpp>
#include <xenomods/menu/Menu.hpp>

#include "xenomods/engine/fw/UpdateInfo.hpp"

namespace xenomods {

	/**
	 * A module.
	 */
	struct UpdatableModule {
		virtual ~UpdatableModule() = default;

		virtual void Initialize() {
			HasInitialized = true;
		};
		bool HasInitialized = false;

		virtual bool NeedsUpdate() const {
			return false;
		}

		virtual void Update(fw::UpdateInfo* updateInfo) {
		}

		virtual void OnConfigUpdate() {
		}

		virtual void OnMapChange(unsigned short mapId) {
		}
	};

	namespace detail {
		void RegisterModule(const char* name, UpdatableModule* module);

		bool IsModuleRegistered(const std::string& moduleName);
	}

	template<class TUpdatable>
	struct ModuleRegistrar {
		static_assert(std::is_base_of_v<UpdatableModule, TUpdatable>, "what are you doing");

		explicit ModuleRegistrar(const char* name, TUpdatable* updatable) {
			if(std::string_view(name).starts_with("xenomods::"))
				detail::RegisterModule(name + 10, updatable);
			else
				detail::RegisterModule(name, updatable);
		}
	};

#define XENOMODS_REGISTER_MODULE(T) \
	static T module__##T;          \
	::xenomods::ModuleRegistrar<T> register__##T(#T, &module__##T);

	void InitializeAllRegisteredModules();

	/**
	 * Update all registered modules.
	 */
	void UpdateAllRegisteredModules(fw::UpdateInfo* updateInfo);

	/**
	 * Marker for doing stuff after the config updates for all registered modules.
	 * NOTE: Only done on a successful config reload.
	 */
	void ConfigUpdateForAllRegisteredModules();

	/**
	 * Notifies all modules about a map change.
	 */
	void MapChangeForAllRegisteredModules(unsigned short mapId);
} // namespace xenomods
