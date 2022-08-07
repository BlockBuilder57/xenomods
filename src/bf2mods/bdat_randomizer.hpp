#pragma once

#include <fmt/core.h>

#include <cstdint>

#include "bf2mods/UpdatableModule.hpp"

namespace bf2mods {

	struct BdatRandomizer : public bf2mods::UpdatableModule {
		enum class BdatScrambleType : std::uint8_t {
			Off,
			ScrambleIndex,
			ShowSheetName,
			Count
		};

		static BdatScrambleType scrambleType;

		void Initialize() override;
		bool NeedsUpdate() const override {
			return true;
		}
		void Update() override;
	};

} // namespace bf2mods

template<>
struct fmt::formatter<bf2mods::BdatRandomizer::BdatScrambleType> : fmt::formatter<std::string_view> {
	template<typename FormatContext>
	inline auto format(bf2mods::BdatRandomizer::BdatScrambleType type, FormatContext& ctx) {
		std::string_view name;

		// clang-format off
		switch(type) {
			using enum bf2mods::BdatRandomizer::BdatScrambleType;

			case ScrambleIndex: name = "Scramble Index"; break;
			case ShowSheetName: name = "Show Bdat Sheet Name"; break;
			case Off: name = "Disabled"; break;
			case Count: default: name = "Unknown"; break;
		}
		// clang-format on

		return formatter<std::string_view>::format(name, ctx);
	}
};