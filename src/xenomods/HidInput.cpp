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

		//std::string toastTag = fmt::format("player{}", padId+1);
		//g_Logger->ToastDebug(toastTag + "styleSet", "P{} StyleSet: {:#05b}", padId+1, styleSet.field[0]);

		if (styleSet.isBitSet(nn::hid::NpadStyleTag::NpadStyleFullKey)) {
			nn::hid::GetNpadState(&reinterpret_cast<nn::hid::NpadFullKeyState&>(padState), padId);
			//g_Logger->ToastDebug(toastTag + "state", "P{} is a FullKey", padId+1);
		}
		else if (styleSet.isBitSet(nn::hid::NpadStyleTag::NpadStyleHandheld)) {
			nn::hid::GetNpadState(&reinterpret_cast<nn::hid::NpadHandheldState&>(padState), padId);
			//g_Logger->ToastDebug(toastTag + "state", "P{} is a Handheld", padId+1);
		}
		else if (styleSet.isBitSet(nn::hid::NpadStyleTag::NpadStyleJoyDual)) {
			nn::hid::GetNpadState(&reinterpret_cast<nn::hid::NpadJoyDualState&>(padState), padId);
			//g_Logger->ToastDebug(toastTag + "state", "P{} is a JoyDual", padId+1);
		}

		// handheld mode screws a few things up, let's handle it properly (for real this time)
		if (this == &controllers[0] && nn::oe::GetOperationMode() == nn::oe::OperationMode::Handheld) {
			// player 1 is not connected, let's check the handheld
			nn::hid::NpadHandheldState handheldPadState {};
			nn::hid::GetNpadState(&handheldPadState, 0x20);
			// logical or the handheld values if connected, this is what libnx does
			if (handheldPadState.mAttributes.isBitSet(nn::hid::NpadAttribute::IsConnected)) {
				//g_Logger->ToastDebug("player1handheld", "P1: adding handheld");

				padState.mAttributes.field[0] |= handheldPadState.mAttributes.field[0];
				padState.mButtons.field[0] |= handheldPadState.mButtons.field[0];

				if (handheldPadState.mAnalogStickL.X != 0 || handheldPadState.mAnalogStickL.Y)
					padState.mAnalogStickL = handheldPadState.mAnalogStickL;
				if (handheldPadState.mAnalogStickR.X != 0 || handheldPadState.mAnalogStickR.Y)
					padState.mAnalogStickR = handheldPadState.mAnalogStickR;
			}
		}

		//g_Logger->ToastDebug(toastTag + "attribute", "P{} Attribute: {:#05b}", padId+1, padState.mAttributes.field[0]);

		if(padState.mAttributes.isBitSet(nn::hid::NpadAttribute::IsConnected)) {
			stateCur.Buttons = std::bit_cast<std::uint64_t>(padState.mButtons.field);
			stateCur.LAxis = glm::vec2(static_cast<float>(padState.mAnalogStickL.X) / 32768.f, static_cast<float>(padState.mAnalogStickL.Y) / 32768.f);
			stateCur.RAxis = glm::vec2(static_cast<float>(padState.mAnalogStickR.X) / 32768.f, static_cast<float>(padState.mAnalogStickR.Y) / 32768.f);
			padConnected = true;
		}
		else {
			//g_Logger->ToastError(toastTag + "disconnect", "PLAYER {} NOT CONNECTED", padId+1);
			padConnected = false;
		}
	}

	HidInput* HidInput::GetPlayer(int player) {
		return &controllers[std::clamp(player, 1, CONTROLLER_COUNT) - 1];
	}

	HidInput* HidInput::GetDebugInput() {
		return controllers[CONTROLLER_COUNT - 1].padConnected ? &controllers[CONTROLLER_COUNT - 1] : &controllers[0];
	}

} // namespace xenomods