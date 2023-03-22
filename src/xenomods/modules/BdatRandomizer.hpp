#pragma once

#include <cstdint>

#include "BdatOverride.hpp"
#include "UpdatableModule.hpp"
#include "fmt/core.h"

namespace xenomods {

	struct BdatRandomizer : public UpdatableModule {
		enum class BdatMSScrambleType : std::uint8_t {
			Off,
			ScrambleIndex,
			ShowSheetName,
			Count
		};

		static BdatMSScrambleType msScrambleType;

		void Initialize() override;
		bool NeedsUpdate() const override {
			return true;
		}
		void Update(fw::UpdateInfo* updateInfo) override;
	};

} // namespace xenomods

template<>
struct fmt::formatter<xenomods::BdatRandomizer::BdatMSScrambleType> : fmt::formatter<std::string_view> {
	template<typename FormatContext>
	inline auto format(xenomods::BdatRandomizer::BdatMSScrambleType type, FormatContext& ctx) {
		std::string_view name;

		// clang-format off
		switch(type) {
			using enum xenomods::BdatRandomizer::BdatMSScrambleType;

			case ScrambleIndex: name = "Scramble Index"; break;
			case ShowSheetName: name = "Show Sheet/Label Name"; break;
			case Off: name = "Disabled"; break;
			case Count: default: name = "Unknown"; break;
		}
		// clang-format on

		return formatter<std::string_view>::format(name, ctx);
	}
};