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

	HidInput* GetPlayer(int player);

	enum class Keybind : std::uint64_t {
		RELOAD_CONFIG = nn::hid::KEY_PLUS,

		MENU_OPEN = nn::hid::KEY_L | nn::hid::KEY_R | nn::hid::KEY_ZL | nn::hid::KEY_ZR,
		MENU_UP = nn::hid::KEY_DUP,
		MENU_DOWN = nn::hid::KEY_DDOWN,
		MENU_SELECT = nn::hid::KEY_DRIGHT,
		MENU_BACK = nn::hid::KEY_DLEFT,

		MENU_NUM_INC = nn::hid::KEY_DUP,
		MENU_NUM_DEC = nn::hid::KEY_DDOWN,
		MENU_NUM_MULT2 = nn::hid::KEY_B,
		MENU_NUM_TENS = nn::hid::KEY_A,
		MENU_NUM_NEGATE = nn::hid::KEY_Y,

		BDAT_MSSCRAMBLETYPE_SWITCH = nn::hid::KEY_X,

		MOVEMENT_SPEED_UP = nn::hid::KEY_SL, // temporary, will be removed
		MOVEMENT_SPEED_DOWN = nn::hid::KEY_SR,
		SAVE_WARP = nn::hid::KEY_DLEFT | nn::hid::KEY_ZL | nn::hid::KEY_L,
		LOAD_WARP = nn::hid::KEY_DRIGHT | nn::hid::KEY_ZL | nn::hid::KEY_L,

		TEST_BUTTON = nn::hid::KEY_MINUS | nn::hid::KEY_A,
		TEMPINT_INC = nn::hid::KEY_DRIGHT | nn::hid::KEY_ZL,
		TEMPINT_DEC = nn::hid::KEY_DLEFT | nn::hid::KEY_ZL,

		MAPJUMP_JUMP = nn::hid::KEY_DDOWN | nn::hid::KEY_ZL,
		ACCESS_CLOSED_LANDMARKS = nn::hid::KEY_DUP | nn::hid::KEY_ZL,
		PLAYSE = nn::hid::KEY_MINUS | nn::hid::KEY_ZL,

		MOONJUMP = nn::hid::KEY_R | nn::hid::KEY_B,
		DISABLE_FALL_DAMAGE = nn::hid::KEY_A,

		RETURN_TO_TITLE = nn::hid::KEY_SL | nn::hid::KEY_SR | nn::hid::KEY_ZL | nn::hid::KEY_ZR, // you too

		UI_RENDER_TOGGLE = nn::hid::KEY_Y,
		DEBUG_RENDER_TOGGLE = nn::hid::KEY_B,

		STRAIGHTEN_FONT = nn::hid::KEY_L | nn::hid::KEY_Y,

		EVENT_DEBUG_PREV = nn::hid::KEY_R | nn::hid::KEY_DLEFT,
		EVENT_DEBUG_NEXT = nn::hid::KEY_R | nn::hid::KEY_DRIGHT,
		EVENT_DEBUG_TOGGLE = nn::hid::KEY_R | nn::hid::KEY_DUP,

		CAMERA_COMBO = nn::hid::KEY_L | nn::hid::KEY_R,
		FREECAM_TOGGLE = nn::hid::KEY_LSTICK | nn::hid::KEY_RSTICK,

		FREECAM_FOVRESET = CAMERA_COMBO | nn::hid::KEY_LSTICK,
		FREECAM_ROTRESET = CAMERA_COMBO | nn::hid::KEY_RSTICK,
		FREECAM_SPEED_UP = CAMERA_COMBO | nn::hid::KEY_PLUS,
		FREECAM_SPEED_DOWN = CAMERA_COMBO | nn::hid::KEY_MINUS,
		FREECAM_TELEPORT = CAMERA_COMBO | nn::hid::KEY_B,

		CAMERA_RENDER_TOGGLE_1 = CAMERA_COMBO | nn::hid::KEY_DUP,
		CAMERA_RENDER_TOGGLE_2 = CAMERA_COMBO | nn::hid::KEY_DLEFT,
		CAMERA_RENDER_TOGGLE_3 = CAMERA_COMBO | nn::hid::KEY_DRIGHT,
		CAMERA_RENDER_TOGGLE_4 = CAMERA_COMBO | nn::hid::KEY_DDOWN,
		CAMERA_RENDER_TOGGLE_5 = CAMERA_COMBO | nn::hid::KEY_Y,
		CAMERA_RENDER_TOGGLE_6 = CAMERA_COMBO | nn::hid::KEY_X,
		CAMERA_RENDER_TOGGLE_7 = CAMERA_COMBO | nn::hid::KEY_A,

		LOGGER_TEST = nn::hid::KEY_PLUS | nn::hid::KEY_R,
		DISPLAY_VERSION = nn::hid::KEY_PLUS | nn::hid::KEY_MINUS,
	};

}; // namespace xenomods
