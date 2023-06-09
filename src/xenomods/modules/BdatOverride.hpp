//
// Created by block on 2/7/2023.
//

#pragma once

#include <toml++/toml.h>

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

#include "UpdatableModule.hpp"

namespace xenomods {

	/**
	 * Interface for Bdat overrides to follow.
	 */
	struct BdatOverrideBase {
		struct SheetData {
			unsigned char* buffer;
			const char* name;
			const char* member;
			unsigned short row;
		};

		struct Access {
			SheetData sheet;
			unsigned long data {};
		};

		[[nodiscard]] virtual bool IsApplicable(SheetData& sheetName) const = 0;
		virtual void operator()(Access& access) = 0;
	};

	struct BdatOverride : public UpdatableModule {
		static std::vector<BdatOverrideBase*> Callbacks;
		static toml::table TOMLTable;
		static std::vector<std::string> BlockedSheets;
		static std::unordered_map<std::string_view, unsigned short> SheetMaxIDs;
		//static std::unordered_map<std::string_view, unsigned long> HotPath;

		void Initialize() override;
		void OnConfigUpdate() override;

		static void RegisterCallback(xenomods::BdatOverrideBase* override);
		static void LoadFromFile();

	};

} // namespace xenomods
