#pragma once

#include <type_traits>

#include <xenomods/State.hpp>

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
