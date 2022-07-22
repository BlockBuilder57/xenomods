#pragma once

#include "bf2mods/mm/math_types.hpp"
#include "bf2mods/prettyprinter.hpp"
#include "bf2mods/utils.hpp"

namespace bf2mods {

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

		bool disableFallDamage;
		float movementSpeedMult;
		bool enableUIRendering;
	};

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

	struct Bf2ModsState {

		explicit Bf2ModsState();

		/**
		 * Reset to clean state.
		 */
		void Reset();

		/**
		 * Current options.
		 */
		Options options{};

		// declare this in camera_tools?
		struct Freecam {
			bool isOn;
			mm::Mat44 matrix;
			float fov;
			float camSpeed;
		} freecam{};

		int mapjumpId{};

		bool moonJump{};
	};


	/**
	 * Get singleton state.
	 */
	Bf2ModsState& GetState();


} // namespace bf2mods
