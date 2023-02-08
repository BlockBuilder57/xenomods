//
// Created by block on 2/7/2023.
//

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "UpdatableModule.hpp"

namespace bf2mods {

	/**
	 * Interface for Bdat overrides to follow.
	 */
	struct BdatOverrideBase {
		struct SheetData {
			unsigned char* buffer;
			std::string_view name;
			std::string_view member;
			unsigned short row;
		};

		struct Access {
			SheetData sheet;
			void* data {};
		};

		[[nodiscard]] virtual bool IsApplicable(SheetData& sheetName) const = 0;
		virtual void operator()(Access& access) = 0;
	};

	struct BdatOverride : public UpdatableModule {
		static std::vector<BdatOverrideBase*> Callbacks;

		static void RegisterCallback(bf2mods::BdatOverrideBase* override);
		void Initialize() override;
	};

} // namespace bf2mods
