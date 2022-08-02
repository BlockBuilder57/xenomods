#include <bf2mods/UpdatableModule.hpp>
#include <array>

namespace bf2mods {

	constexpr auto MAX_MODULES = 16;

	//std::vector<UpdatableModule*> registeredModules;

	std::array<UpdatableModule*, MAX_MODULES> registeredModules;
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

		void RegisterModule(UpdatableModule* module) {
			if(module == nullptr)
				return;

			//if(registeredModules == nullptr)
			//	ModuleInit();

			registeredModules[moduleIndex++] = module;
		}

	}

	void InitializeAllRegisteredModules() {
		for(int i = 0 ; i < moduleIndex; ++i)
			registeredModules[i]->Initialize();
	}

	void UpdateAllRegisteredModules() {
		for(int i = 0 ; i < moduleIndex; ++i)
			if(registeredModules[i]->NeedsUpdate())
				registeredModules[i]->Update();
			else
				continue;
	}

}