//
// Created by block on 1/19/2023.
//

#include <nn/oe.h>

#include "FunctionalHooks.hpp"

#include <xenomods/HidInput.hpp>
#include <xenomods/Logger.hpp>

namespace xenomods {

	constexpr static auto CONTROLLER_COUNT = 2;

	static HidInput controllers[CONTROLLER_COUNT]{
		{ nn::hid::CONTROLLER_PLAYER_1 },
		{ nn::hid::CONTROLLER_PLAYER_2 },
	};

	void HidInput::Poll() {
		statePrev = stateCur;

		// nintendo why is it like this
		if (nn::oe::GetOperationMode() == nn::oe::OperationMode::Handheld) {
			// player 1 will always be 0 if we're stuck to the handheld controller
			if (padId == nn::hid::CONTROLLER_PLAYER_1 || padId == nn::hid::CONTROLLER_HANDHELD)
				padId = nn::hid::GetNpadStyleSet(nn::hid::CONTROLLER_PLAYER_1).flags > 0 ? nn::hid::CONTROLLER_PLAYER_1 : nn::hid::CONTROLLER_HANDHELD;
		}
		else if (padId == nn::hid::CONTROLLER_HANDHELD)
			padId = nn::hid::CONTROLLER_PLAYER_1;

		nn::hid::NpadState padState {};
		auto styleSet = nn::hid::GetNpadStyleSet(padId);

		//g_Logger->LogInfo("Player {} has styleset {:#05b}", padId+1, styleSet.flags);

		if (styleSet.flags & nn::hid::NpadStyleFullKey.flags)
			nn::hid::GetNpadState(&reinterpret_cast<nn::hid::NpadFullKeyState&>(padState), padId);
		else if (styleSet.flags & nn::hid::NpadStyleHandheld.flags)
			nn::hid::GetNpadState(&reinterpret_cast<nn::hid::NpadHandheldState&>(padState), padId);
		else if (styleSet.flags & nn::hid::NpadStyleJoyDual.flags)
			nn::hid::GetNpadState(&reinterpret_cast<nn::hid::NpadJoyDualState&>(padState), padId);

		if(padState.Flags & nn::hid::NpadFlags::NPAD_CONNECTED) {
			stateCur.Buttons = padState.Buttons;
			stateCur.LAxis = glm::vec2(static_cast<float>(padState.LStickX) / 32768.f, static_cast<float>(padState.LStickY) / 32768.f);
			stateCur.RAxis = glm::vec2(static_cast<float>(padState.RStickX) / 32768.f, static_cast<float>(padState.RStickY) / 32768.f);
		}
		else {
			//g_Logger->LogError("PLAYER {} NOT CONNECTED", padId+1);
		}
	}

	HidInput* HidInput::GetPlayer(int player) {
		return &controllers[std::clamp(player, 1, CONTROLLER_COUNT) - 1];
	}

	int HidInput::GetDebugInputNum() {
		return ClampNumberOfControllers::HasApplied() ? ClampNumberOfControllers::Orig() : 1;
	}
	HidInput* HidInput::GetDebugInput() {
		return GetPlayer(GetDebugInputNum());
	}

} // namespace xenomods