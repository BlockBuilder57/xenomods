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

}; // namespace xenomods
