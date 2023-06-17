#pragma once

#include <type_traits>

#include <skylaunch/hookng/Hooks.hpp>
#include <xenomods/DebugWrappers.hpp>
#include <xenomods/HidInput.hpp>
#include <xenomods/Logger.hpp>
#include <xenomods/NnFile.hpp>
#include <xenomods/State.hpp>
#include <xenomods/Utils.hpp>
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
	};

	namespace detail {
		void RegisterModule(UpdatableModule* module);

		static bool IsModuleRegistered(const char* moduleName)
		{
			std::string strModuleName = std::string(moduleName);
			// dumb hack (as if this whole function isn't)
			if (strModuleName.starts_with("xenomods::"))
				strModuleName = strModuleName.substr(10);

			std::string symbol = "_ZN8xenomods15ModuleRegistrarINS_";
			symbol += std::to_string(strModuleName.size());
			symbol += strModuleName;
			symbol += "EEC1EPS1_";
			return skylaunch::hook::detail::ResolveSymbolBase(symbol) != skylaunch::hook::INVALID_FUNCTION_PTR;
		}
	}

	template<class TUpdatable>
	struct ModuleRegistrar {
		static_assert(std::is_base_of_v<UpdatableModule, TUpdatable>, "what are you doing");

		explicit ModuleRegistrar(TUpdatable* updatable) {
			detail::RegisterModule(updatable);
		}
	};

#define XENOMODS_REGISTER_MODULE(T) \
	static T module__##T;          \
	::xenomods::ModuleRegistrar<T> register__##T(&module__##T);

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
} // namespace xenomods
