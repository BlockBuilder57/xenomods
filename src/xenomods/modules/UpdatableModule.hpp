#pragma once

#include <type_traits>

#include "../State.hpp"

namespace xenomods {

	/**
	 * A module.
	 */
	struct UpdatableModule {
		virtual ~UpdatableModule() = default;

		virtual void Initialize() = 0;

		virtual bool NeedsUpdate() const {
			return false;
		}

		virtual void Update() {
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
	void UpdateAllRegisteredModules();

	/**
	 * Marker for doing stuff after the config updates for all registered modules.
	 * NOTE: Only done on a successful config reload.
	 */
	void ConfigUpdateForAllRegisteredModules();
} // namespace xenomods
