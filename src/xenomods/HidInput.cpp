//
// Created by block on 1/19/2023.
//

#include <nn/oe.h>

#include <xenomods/DebugWrappers.hpp>
#include <xenomods/HidInput.hpp>
#include <xenomods/Logger.hpp>

#include "FunctionalHooks.hpp"

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

	void HidInput::DebugDraw() {
		HidInput* P1 = HidInput::GetPlayer(1);
		HidInput* P2 = HidInput::GetPlayer(2);

		std::string p1Buttons = P1->padConnected ? fmt::format("{:#08x} - P1 - {:#08x}", P1->stateCur.Buttons, P1->statePrev.Buttons) : "P1 Disconnected";
		std::string p2Buttons = P2->padConnected ? fmt::format("{:#08x} - P2 - {:#08x}", P2->stateCur.Buttons, P2->statePrev.Buttons) : "P2 Disconnected";
		int buttonsP1Width = xenomods::debug::drawFontGetWidth(p1Buttons.c_str());
		int buttonsP2Width = xenomods::debug::drawFontGetWidth(p2Buttons.c_str());
		xenomods::debug::drawFontShadow((1280*1/4)-(buttonsP1Width/2), 16, {1,1,1,1}, p1Buttons.c_str());
		xenomods::debug::drawFontShadow((1280*3/4)-(buttonsP2Width/2), 16, {1,1,1,1}, p2Buttons.c_str());
		xenomods::debug::drawFontShadow((1280*((HidInput::GetDebugInput()->padId * 2) + 1)/4), 32, {0,1,1,1}, "^ debug");

		/*nn::hid::NpadButton testcombo = static_cast<nn::hid::NpadButton>(1);
		if(P1->InputHeld(testcombo))
			xenomods::debug::drawFontShadow(1280/2, 32, {0,1,1,1}, "combo held!");
		if (P1->InputDown(testcombo))
			g_Logger->LogDebug("combo down...");
		if (P1->InputUp(testcombo))
			g_Logger->LogDebug("combo up!");
		if(P1->InputHeldStrict(testcombo))
			xenomods::debug::drawFontShadow(1280/2, 48, {0,1,1,1}, "strict combo held!");
		if (P1->InputDownStrict(testcombo))
			g_Logger->LogDebug("strict combo down...");
		if (P1->InputUpStrict(testcombo))
			g_Logger->LogDebug("strict combo up!");*/
	}

} // namespace xenomods