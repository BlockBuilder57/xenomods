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
		{ 0 },
		{ 1 },
	};

	void HidInput::Poll() {
		statePrev = stateCur;

		nn::hid::NpadBaseState padState {};
		auto styleSet = nn::hid::GetNpadStyleSet(padId);

		//g_Logger->LogInfo("Player {} has styleset {:#05b}", padId+1, styleSet.flags);

		if (styleSet.isBitSet(nn::hid::NpadStyleTag::NpadStyleFullKey))
			nn::hid::GetNpadState(&reinterpret_cast<nn::hid::NpadFullKeyState&>(padState), padId);
		else if (styleSet.isBitSet(nn::hid::NpadStyleTag::NpadStyleHandheld))
			nn::hid::GetNpadState(&reinterpret_cast<nn::hid::NpadHandheldState&>(padState), padId);
		else if (styleSet.isBitSet(nn::hid::NpadStyleTag::NpadStyleJoyDual))
			nn::hid::GetNpadState(&reinterpret_cast<nn::hid::NpadJoyDualState&>(padState), padId);

		if(padState.mAttributes.isBitSet(nn::hid::NpadAttribute::IsConnected)) {
			stateCur.Buttons = std::bit_cast<std::uint64_t>(padState.mButtons.field);
			stateCur.LAxis = glm::vec2(static_cast<float>(padState.mAnalogStickL.X) / 32768.f, static_cast<float>(padState.mAnalogStickL.Y) / 32768.f);
			stateCur.RAxis = glm::vec2(static_cast<float>(padState.mAnalogStickR.X) / 32768.f, static_cast<float>(padState.mAnalogStickR.Y) / 32768.f);
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