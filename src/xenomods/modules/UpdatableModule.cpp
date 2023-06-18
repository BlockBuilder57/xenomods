#include "UpdatableModule.hpp"

#include <array>

namespace xenomods {

	constexpr auto MAX_MODULES = 16;

	//std::vector<UpdatableModule*> registeredModules;

	// TODO: figure out how to use construct on first use idiom to just use a map
	// this is just so this code doesnt need to be completely changed yet
	struct RegisteredModule {
		std::string_view name;
		UpdatableModule* modulePtr{};
	};

	std::array<RegisteredModule, MAX_MODULES> registeredModules;
	int moduleIndex = 0;

	namespace detail {

		void ModuleInit() {
			// create and allocate some module pointers, to hopefully
			// avoid allocation
			//registeredModules = new std::vector<UpdatableModule*>();
			//registeredModules->reserve(8);
		}

		void ModuleFini() {
			//	delete registeredModules;
		}

		void RegisterModule(const char* name, UpdatableModule* module) {
			if(module == nullptr)
				return;

			//if(registeredModules == nullptr)
			//	ModuleInit();

			registeredModules[moduleIndex++] = {
				name,
				module
			};
		}

		bool IsModuleRegistered(const std::string& moduleName) {
			std::string_view view = moduleName;
			if (view.starts_with("xenomods::"))
				view.remove_prefix(10);

			for(int i = 0; i < moduleIndex; ++i)
				if(registeredModules[i].name == view)
					return true;
			return false;
		}


	} // namespace detail

	void InitializeAllRegisteredModules() {
		for(int i = 0; i < moduleIndex; ++i)
			registeredModules[i].modulePtr->Initialize();
	}

	void UpdateAllRegisteredModules(fw::UpdateInfo* updateInfo) {
		for(int i = 0; i < moduleIndex; ++i)
			if(registeredModules[i].modulePtr->NeedsUpdate())
				registeredModules[i].modulePtr->Update(updateInfo);
	}

	void ConfigUpdateForAllRegisteredModules() {
		for(int i = 0; i < moduleIndex; ++i)
			if(registeredModules[i].modulePtr->HasInitialized)
				registeredModules[i].modulePtr->OnConfigUpdate();
	}

	void MapChangeForAllRegisteredModules(unsigned short mapId) {
		for(int i = 0; i < moduleIndex; ++i)
			if(registeredModules[i].modulePtr->HasInitialized)
				registeredModules[i].modulePtr->OnMapChange(mapId);
	}

} // namespace xenomods