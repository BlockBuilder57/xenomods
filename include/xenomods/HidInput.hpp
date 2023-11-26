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

		int padId {};
		bool padConnected {};

		constexpr HidInput(int id)
			: padId(id) {

		}

		void Poll();

		static HidInput* GetPlayer(int player);

		/*
		 * Gets the controller used for the debug input (like the Menu)
		 * Uses the last numbered controller if it is connected, otherwise P1.
		 */
		static HidInput* GetDebugInput();

		static void DebugDraw();

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

	using enum nn::hid::NpadButton;
#define NPADBIT(n) (1UL << ((std::uint64_t)n))

	enum class Keybind : std::uint64_t {
		// in-game controls
		MOONJUMP = NPADBIT(R) | NPADBIT(B),

		EVENT_DEBUG_PREV = NPADBIT(R) | NPADBIT(Left),
		EVENT_DEBUG_NEXT = NPADBIT(R) | NPADBIT(Right),
		EVENT_DEBUG_TOGGLE = NPADBIT(R) | NPADBIT(Up),

		// menu controls + binds
		MENU_TOGGLE = NPADBIT(L) | NPADBIT(R) | NPADBIT(ZL) | NPADBIT(ZR),
		MENU_UP = NPADBIT(Up),
		MENU_DOWN = NPADBIT(Down),
		MENU_SELECT = NPADBIT(Right),
		MENU_BACK = NPADBIT(Left),

		MENU_NUM_INC = NPADBIT(Up),
		MENU_NUM_DEC = NPADBIT(Down),
		MENU_NUM_MULT2 = NPADBIT(B),
		MENU_NUM_NEXT_TENS = NPADBIT(A),
		MENU_NUM_NEGATE = NPADBIT(Y),
		MENU_NUM_SET0 = NPADBIT(X),

		CAMERA_COMBO = NPADBIT(L) | NPADBIT(R),
		FREECAM_TOGGLE = NPADBIT(StickL) | NPADBIT(StickR),

		FREECAM_FOVRESET = CAMERA_COMBO | NPADBIT(StickL),
		FREECAM_ROTRESET = CAMERA_COMBO | NPADBIT(StickR),
		FREECAM_SPEED_UP = CAMERA_COMBO | NPADBIT(Plus),
		FREECAM_SPEED_DOWN = CAMERA_COMBO | NPADBIT(Minus),
		FREECAM_TELEPORT = CAMERA_COMBO | NPADBIT(B),
	};

#undef NPADBIT

}; // namespace xenomods
