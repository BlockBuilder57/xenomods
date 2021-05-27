#pragma once

#include "nn/hid.hpp"

namespace util {

	struct NpadScanner {
		bool useHandheldStyle = {};
		nn::hid::NpadId npadId = {};
		nn::hid::NpadFullKeyState keyState = {};
		decltype(keyState.Buttons) privButtons = {};

		void scanInput() {
			privButtons = keyState.Buttons;

			auto handheldNpadId = nn::hid::CONTROLLER_HANDHELD;
			auto npadStyleSet = nn::hid::GetNpadStyleSet(handheldNpadId);
			if(useHandheldStyle and npadStyleSet.flags & nn::hid::NpadStyleHandheld.flags) {
				nn::hid::GetNpadState(static_cast<nn::hid::NpadHandheldState*>(&keyState), nn::hid::CONTROLLER_HANDHELD);
			} else {
				auto npadStyleSet = nn::hid::GetNpadStyleSet(npadId);
				if(npadStyleSet.flags & nn::hid::NpadStyleFullKey.flags) {
					nn::hid::GetNpadState(reinterpret_cast<nn::hid::NpadFullKeyState*>(&keyState), npadId);
				} else if(npadStyleSet.flags & nn::hid::NpadStyleJoyDual.flags) {
					nn::hid::GetNpadState(reinterpret_cast<nn::hid::NpadJoyDualState*>(&keyState), npadId);
				}
			}
		}

		inline bool keyComboJustPressed(decltype(keyState.Buttons) keyCombo) {
			return ((keyState.Buttons & keyCombo) == keyCombo) and not((privButtons & keyCombo) == keyCombo);
		}
	};

} // namespace util
