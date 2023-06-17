//
// Created by block on 1/19/2023.
//

#pragma once

#include <xenomods/engine/mm/MathTypes.hpp>
#include <nn/hid.hpp>

namespace xenomods {

	struct HidInput {
		struct State {
			std::uint64_t Buttons;
			glm::vec2 LAxis;
			glm::vec2 RAxis;
		};

		State stateCur {};
		State statePrev {};

		nn::hid::NpadId padId {};

		constexpr HidInput(nn::hid::NpadId id)
			: padId(id) {

		}

		void Poll();

		static HidInput* GetPlayer(int player);

		/*
		 * Gets the controller used for the debug input (like the Menu)
		 * Uses the last numbered controller if it is connected, otherwise P1.
		 */
		static HidInput* GetDebugInput();

		/**
		 * \defgroup input Input functions
		 * Standard style input functions. Can take any kind of integer to check.
		 * Strict functions ensure that only the specified combo is allowed to be held down.
		 * @{
		 */

		template<class T>
		inline bool InputHeld(T combo) {
			return bitMask(static_cast<T>(stateCur.Buttons), combo);
		}

		template<class T>
		inline bool InputUp(T combo) {
			return !bitMask(static_cast<T>(stateCur.Buttons), combo) && bitMask(static_cast<T>(statePrev.Buttons), combo);
		}

		template<class T>
		inline bool InputDown(T combo) {
			return bitMask(static_cast<T>(stateCur.Buttons), combo) && !bitMask(static_cast<T>(statePrev.Buttons), combo);
		}

		template<class T>
		inline bool InputHeldStrict(T combo) {
			return bitMaskStrict(static_cast<T>(stateCur.Buttons & 0xFFFF), combo);
		}

		template<class T>
		inline bool InputUpStrict(T combo) {
			return !bitMaskStrict(static_cast<T>(stateCur.Buttons & 0xFFFF), combo) && bitMaskStrict(static_cast<T>(statePrev.Buttons & 0xFFFF), combo);
		}

		template<class T>
		inline bool InputDownStrict(T combo) {
			return bitMaskStrict(static_cast<T>(stateCur.Buttons & 0xFFFF), combo) && !bitMaskStrict(static_cast<T>(statePrev.Buttons & 0xFFFF), combo);
		}

		/** @} */
	};



	enum class Keybind : std::uint64_t {
		// in-game controls
		MOONJUMP = nn::hid::KEY_R | nn::hid::KEY_B,

		EVENT_DEBUG_PREV = nn::hid::KEY_R | nn::hid::KEY_DLEFT,
		EVENT_DEBUG_NEXT = nn::hid::KEY_R | nn::hid::KEY_DRIGHT,
		EVENT_DEBUG_TOGGLE = nn::hid::KEY_R | nn::hid::KEY_DUP,

		// menu controls + binds
		MENU_TOGGLE = nn::hid::KEY_L | nn::hid::KEY_R | nn::hid::KEY_ZL | nn::hid::KEY_ZR,
		MENU_UP = nn::hid::KEY_DUP,
		MENU_DOWN = nn::hid::KEY_DDOWN,
		MENU_SELECT = nn::hid::KEY_DRIGHT,
		MENU_BACK = nn::hid::KEY_DLEFT,

		MENU_NUM_INC = nn::hid::KEY_DUP,
		MENU_NUM_DEC = nn::hid::KEY_DDOWN,
		MENU_NUM_MULT2 = nn::hid::KEY_B,
		MENU_NUM_TENS = nn::hid::KEY_A,
		MENU_NUM_NEGATE = nn::hid::KEY_Y,
		MENU_NUM_SET0 = nn::hid::KEY_X,

		CAMERA_COMBO = nn::hid::KEY_L | nn::hid::KEY_R,
		FREECAM_TOGGLE = nn::hid::KEY_LSTICK | nn::hid::KEY_RSTICK,

		FREECAM_FOVRESET = CAMERA_COMBO | nn::hid::KEY_LSTICK,
		FREECAM_ROTRESET = CAMERA_COMBO | nn::hid::KEY_RSTICK,
		FREECAM_SPEED_UP = CAMERA_COMBO | nn::hid::KEY_PLUS,
		FREECAM_SPEED_DOWN = CAMERA_COMBO | nn::hid::KEY_MINUS,
	};

}; // namespace xenomods
