#pragma once

#include "bf2mods/mm/math_types.hpp"
#include "prettyprinter.hpp"
#include "utils.hpp"
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

	// TODO: move to a new file. probably.

	struct Options {
		enum class BdatScrambleType : std::uint8_t {
			Off,
			ScrambleIndex,
			ShowSheetName,
			Count
		};

		/**
		 * Reset to default options.
		 */
		void Reset();

		BdatScrambleType bdatScrambleType;
		//bool bdatMapListResourceOverride;

		bool disableFallDamage;
		float movementSpeedMult;
	};

	// TODO: remove the notion of "shared state" since we don't really need it at the moment.
	// 	If we do, we can reintroduce it.

	struct SharedState {
		explicit SharedState();

		// Reset to default settings (called by constructor to initialize)
		void DefaultSettings();

		Options options;

		// declare this in camera_tools?
		struct Freecam {
			bool isOn;
			mm::Mat44 matrix;
			float fov;
			float camSpeed { 1 };
		} freecam;

		int mapjumpId;

		bool moonJump;
		//bool layerOverwrite;
	};

#define ALIGN_UP(x, a) (((x) + ((a)-1)) & ~((a)-1))
	static constexpr auto SHARED_MEM_SIZE = ALIGN_UP(sizeof(SharedState), 0x1000);

	template<>
	struct Prettyprinter<Options::BdatScrambleType> {
		inline static std::string format(const Options::BdatScrambleType& opt) {
			using enum Options::BdatScrambleType;

			switch(opt) {
				case ScrambleIndex:
					return "Scramble Index";
					break;
				case ShowSheetName:
					return "Show Bdat Sheet Name";
					break;
				case Off:
				default:
					return "Disabled";
					break;
			}
		}

		inline static std::string_view type_name() {
			return "BdatOptions::ScrambleType";
		}
	};

} // namespace bf2mods
