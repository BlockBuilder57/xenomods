#pragma once

#include <type_traits>

namespace bf2mods {

	/**
	 * A module.
	 */
	struct UpdatableModule {
		virtual ~UpdatableModule() = default;

		virtual void Initialize() = 0;

		virtual bool NeedsUpdate() const { return false; }

		virtual void Update() {}
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

#define BF2MODS_REGISTER_MODULE(T) \
		static T module__##T;            \
		::bf2mods::ModuleRegistrar<T> register__##T(&module__##T);


	void InitializeAllRegisteredModules();

	/**
	 * Update all registered modules.
	 */
	void UpdateAllRegisteredModules();
}
