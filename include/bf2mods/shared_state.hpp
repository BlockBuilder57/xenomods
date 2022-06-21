#pragma once

#include "utils.hpp"
#include "prettyprinter.hpp"

#ifdef __SKYLINE__
#include "nn/hid.hpp"

#else
#include <switch.h>
#endif

namespace bf2mods {

#ifdef __SKYLINE__
	using HidControllerKeyData = nn::hid::HidControllerKeys;
#else
	using HidControllerKeyData = HidControllerKeys;
#endif

	STATIC_ASSERT_SIZE(HidControllerKeyData, 4);

	struct SharedState {
		struct Options {
			struct BdatOptions {
				enum class ScrambleType : std::uint8_t {
					Off,
					ScrambleIndex,
					ShowSheetName,
					Count
				} scrambleType;

				bool mapListResourceOverride;
			} bdat;

			struct Gameplay {
				bool disableFallDamage;
				float movementSpeedMult;
			} game;

		} options;

		int mapjumpId;
		int testInt;

		bool moonJump;
		bool layerOverwrite;
	};

#define ALIGN_UP(x, a) (((x) + ((a)-1)) & ~((a)-1))
	static constexpr auto SHARED_MEM_SIZE = ALIGN_UP(sizeof(SharedState), 0x1000);

	template<>
	struct Prettyprinter<SharedState::Options::BdatOptions::ScrambleType> {
		inline static std::string format(const SharedState::Options::BdatOptions::ScrambleType &opt) {
			std::stringstream ss;
			switch (opt) {
				case SharedState::Options::BdatOptions::ScrambleType::ScrambleIndex:
					ss << "Scramble Index";
					break;
				case SharedState::Options::BdatOptions::ScrambleType::ShowSheetName:
					ss << "Show Bdat Sheet Name";
					break;
				case SharedState::Options::BdatOptions::ScrambleType::Off:
				default:
					ss << "Disabled";
					break;
			}
			return ss.str();
		}

		inline static std::string_view type_name() { return "BdatOptions::ScrambleType"; }

	};

} // namespace bf2mods
