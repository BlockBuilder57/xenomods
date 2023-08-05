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
			ShowSheetName
		};

		static BdatMSScrambleType msScrambleType;

		static void MenuSection();

		void Initialize() override;
	};

} // namespace xenomods

template <>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<xenomods::BdatRandomizer::BdatMSScrambleType>(xenomods::BdatRandomizer::BdatMSScrambleType value) noexcept {
	// clang-format off
	switch (value) {
		using enum xenomods::BdatRandomizer::BdatMSScrambleType;

		case ScrambleIndex: return "Scramble Index";
		case ShowSheetName: return "Show Sheet/Label Name";
		case Off: return "Disabled";
	}
	// clang-format on
	return default_tag;
}

template<>
struct fmt::formatter<xenomods::BdatRandomizer::BdatMSScrambleType> : fmt::formatter<std::string_view> {
	template<typename FormatContext>
	inline auto format(xenomods::BdatRandomizer::BdatMSScrambleType type, FormatContext& ctx) {
		using enum xenomods::BdatRandomizer::BdatMSScrambleType;

		if (type < Off || type > ShowSheetName)
			return "Unknown - " + std::to_string(xenomods::underlying_value(type));

		return magic_enum::enum_name(type);
	}
};