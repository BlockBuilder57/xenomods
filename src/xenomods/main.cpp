#include "main.hpp"

#include <skylaunch/hookng/Hooks.hpp>
#include <xenomods/DebugWrappers.hpp>
#include <xenomods/HidInput.hpp>
#include <xenomods/Logger.hpp>
#include <xenomods/NnFile.hpp>
#include <xenomods/State.hpp>
#include <xenomods/Version.hpp>
#include <xenomods/menu/Menu.hpp>

#include "FunctionalHooks.hpp"
#include "nn/diag.h"

#include <imgui_xeno.h>
#include <imgui.h>
#include "modules/RenderingControls.hpp"

namespace xenomods {

	void fmt_assert_failed(const char* file, int line, const char* message) {
		NN_DIAG_LOG(nn::diag::LogSeverity::Fatal, "fmtlib assert caught : %s", message);
	}

	void update(fw::UpdateInfo* updateInfo) {
		// lazy
		using enum xenomods::Keybind;

		HidInput* P1 = HidInput::GetPlayer(1);
		HidInput* P2 = HidInput::GetPlayer(2);
		P1->Poll();
		P2->Poll();

		/*std::string p1Buttons = fmt::format("{:#08x} - P1 - {:#08x}", P1->stateCur.Buttons, P1->statePrev.Buttons);
		std::string p2Buttons = fmt::format("{:#08x} - P2 - {:#08x}", P2->stateCur.Buttons, P2->statePrev.Buttons);
		int buttonsP1Width = xenomods::debug::drawFontGetWidth(p1Buttons.c_str());
		int buttonsP2Width = xenomods::debug::drawFontGetWidth(p2Buttons.c_str());
		xenomods::debug::drawFontShadow(1280-buttonsP1Width-5, 5, mm::Col4::white, p1Buttons.c_str());
		xenomods::debug::drawFontShadow(1280-buttonsP2Width-5, 5+16, mm::Col4::white, p2Buttons.c_str());

		auto testcombo = nn::hid::KEY_A;
		if(P2->InputHeld(testcombo))
			xenomods::debug::drawFontShadow(1280/2, 0, mm::Col4::cyan, "combo held!");
		if (P2->InputDown(testcombo))
			g_Logger->LogDebug("combo down...");
		if (P2->InputUp(testcombo))
			g_Logger->LogDebug("combo up!");
		if(P2->InputHeldStrict(testcombo))
			xenomods::debug::drawFontShadow(1280/2, 16, mm::Col4::cyan, "strict combo held!");
		if (P2->InputDownStrict(testcombo))
			g_Logger->LogDebug("strict combo down...");
		if (P2->InputUpStrict(testcombo))
			g_Logger->LogDebug("strict combo up!");*/

		/*
		 * Check buttons
		 */

		// use P2 if it's connected, otherwise we'll drop P1's input to let them use the menu
		HidInput* menuInput = HidInput::GetDebugInput();
		if(menuInput->InputDownStrict(MENU_TOGGLE)) {
			g_Menu->Toggle();
		}

		// Update modules
		xenomods::UpdateAllRegisteredModules(updateInfo);

		// render the menu if open
		if(g_Menu->IsOpen())
			g_Menu->Update(menuInput);

		// draw logger messages and toasts
		g_Logger->Draw(updateInfo);
	}

	void main() {
		NN_DIAG_LOG(nn::diag::LogSeverity::Info, "Running %s", version::BuildXenomodsVersion());
		g_Menu->Initialize();
		InitializeAllRegisteredModules();

		FunctionalHooks::Hook();
	}

} // namespace xenomods
