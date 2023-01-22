#pragma once

#include <bf2mods/engine/mm/MathTypes.hpp>
#include <nn/hid.hpp>

#include "State.hpp"
#include "modules/UpdatableModule.hpp"

namespace bf2mods {

	enum class Keybind : std::uint64_t {
		CLEAR_TCPLOG = nn::hid::KEY_PLUS,
		BDAT_SCRAMBLETYPE_TOGGLE = nn::hid::KEY_X,

		MOVEMENT_SPEED_UP = nn::hid::KEY_DUP,
		MOVEMENT_SPEED_DOWN = nn::hid::KEY_DDOWN,

		TEST_BUTTON = nn::hid::KEY_MINUS | nn::hid::KEY_A,
		TEMPINT_INC = nn::hid::KEY_DRIGHT | nn::hid::KEY_ZL,
		TEMPINT_DEC = nn::hid::KEY_DLEFT | nn::hid::KEY_ZL,

		MAPJUMP_JUMP = nn::hid::KEY_DDOWN | nn::hid::KEY_ZL,
		PLAYSE = nn::hid::KEY_MINUS | nn::hid::KEY_ZL,

		MOONJUMP = nn::hid::KEY_R | nn::hid::KEY_B,
		DISABLE_FALL_DAMAGE = nn::hid::KEY_A,

		RETURN_TO_TITLE = nn::hid::KEY_L | nn::hid::KEY_R | nn::hid::KEY_ZL | nn::hid::KEY_ZR,

		UI_RENDER_TOGGLE = nn::hid::KEY_Y,
		DEBUG_RENDER_TOGGLE = nn::hid::KEY_B,

		STRAIGHTEN_FONT = nn::hid::KEY_L | nn::hid::KEY_Y,

		EVENT_DEBUG_PREV = nn::hid::KEY_R | nn::hid::KEY_DLEFT,
		EVENT_DEBUG_NEXT = nn::hid::KEY_R | nn::hid::KEY_DRIGHT,
		EVENT_DEBUG_TOGGLE = nn::hid::KEY_R | nn::hid::KEY_DUP,

		FREECAM_TOGGLE = nn::hid::KEY_LSTICK | nn::hid::KEY_RSTICK,
		FREECAM_FOVHOLD = nn::hid::KEY_LSTICK,
		FREECAM_ROLLHOLD = nn::hid::KEY_RSTICK,
		FREECAM_SPEED_UP = nn::hid::KEY_ZL,
		FREECAM_SPEED_DOWN = nn::hid::KEY_L,

		LOGGER_TEST = nn::hid::KEY_PLUS | nn::hid::KEY_R,
	};

	// TODO: make these members of hidinput, and probably store current/previous there (removing the need for "prev" instances).



	/**
 	 * Called on each Framework update.
 	 */
	void update();

	/**
 	 * Called from skylaunch when ready.
 	 */
	void main();

} // namespace bf2mods
