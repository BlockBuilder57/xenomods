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
#include "helpers/InputHelper.h"

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

		//HidInput::DebugDraw();

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
		InputHelper::toggleInput = g_Menu->IsOpen();
		if (g_Menu->IsOpen())
			g_Menu->Update(menuInput);

		// draw logger messages and toasts
		g_Logger->Draw(updateInfo);
	}

	void main() {
		NN_DIAG_LOG(nn::diag::LogSeverity::Info, "Running %s", version::XenomodsFullVersion());
		g_Menu->Initialize();
		InitializeAllRegisteredModules();

		FunctionalHooks::Hook();
	}

} // namespace xenomods
