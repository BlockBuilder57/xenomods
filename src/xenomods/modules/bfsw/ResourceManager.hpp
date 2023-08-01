// Created by block on 7/4/23.

#pragma once

#include "../UpdatableModule.hpp"

#if XENOMODS_CODENAME(bfsw)
namespace xenomods {

	class ResourceManager : public UpdatableModule {
	   public:
		static unsigned short ItemId;
		static unsigned short ItemCount;

		static void MenuSection();

		void Initialize() override;
		bool NeedsUpdate() const override {
			return false;
		}
	};

} // namespace xenomods
#endif
