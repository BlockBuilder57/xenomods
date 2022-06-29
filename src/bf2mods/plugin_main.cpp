#include "plugin_main.hpp"

#include <bf2mods/mm/math_types.hpp>
#include <bf2mods/stuff/utils/debug_util.hpp>

#include "bf2logger.hpp"
#include "plugin.hpp"
#include "skyline/efl/service.hpp"

// all parts here
#include "bdat_randomizer.hpp"
#include "camera_tools.hpp"
#include "debug_stuff.hpp"
#include "menu_viewer.hpp"
#include "player_movement.hpp"

namespace fw {

	GENERATE_SYM_HOOK(Framework_update, "_ZN2fw9Framework6updateEv", void, void* that) {
		Framework_updateBak(that);

		// lazy
		using enum bf2mods::Keybind;
		using bf2mods::p1Cur;
		using bf2mods::p1Prev;
		using bf2mods::p2Cur;
		using bf2mods::p2Prev;


		// Read controllers
		p1Cur.Buttons = 0ul;
		p2Cur.Buttons = 0ul;

		nn::hid::NpadHandheldState npadHandheldState {};
		nn::hid::NpadFullKeyState npadP1FullKeyState {};
		nn::hid::NpadFullKeyState npadP2FullKeyState {};

		nn::hid::GetNpadState(&npadHandheldState, nn::hid::CONTROLLER_HANDHELD);
		nn::hid::GetNpadState(&npadP1FullKeyState, nn::hid::CONTROLLER_PLAYER_1);
		nn::hid::GetNpadState(&npadP2FullKeyState, nn::hid::CONTROLLER_PLAYER_2);

		auto updateHidInput = [&](nn::hid::NpadHandheldState& state, bf2mods::HidInput& cur) {
			if(state.Flags & nn::hid::NpadFlags::NPAD_CONNECTED) {
				cur.Buttons |= state.Buttons;
				cur.LAxis = mm::Vec2 { static_cast<float>(state.LStickX) / 32768.f, static_cast<float>(state.LStickY) / 32768.f };
				cur.RAxis = mm::Vec2 { static_cast<float>(state.RStickX) / 32768.f, static_cast<float>(state.RStickY) / 32768.f };
			}
		};

		updateHidInput(npadHandheldState, p1Cur);
		updateHidInput(npadP1FullKeyState, p1Cur);
		updateHidInput(npadP2FullKeyState, p2Cur);

		//int buttonsP1Width = fw::debug::drawFontGetWidth("%xh - P1", p1Cur.Buttons);
		//int buttonsP2Width = fw::debug::drawFontGetWidth("%xh - P2", p2Cur.Buttons);
		//fw::debug::drawFont(1280-buttonsP1Width-5, 5, &mm::Col4::White, "%xh - P1", p1Cur.Buttons);
		//fw::debug::drawFont(1280-buttonsP2Width-5, 5+16, &mm::Col4::White, "%xh - P2", p2Cur.Buttons);

		/*
		 * Check buttons
		 */

		auto sharedStatePtr = bf2mods::Plugin::getSharedStatePtr();

		sharedStatePtr->moonJump = btnHeld(MOONJUMP, p1Cur.Buttons);

		if(sharedStatePtr->freecam.isOn)
			bf2mods::CameraTools::DoFreeCameraMovement();

		if(!(p2Cur.Buttons & nn::hid::KEY_ZL) && !(p2Cur.Buttons & nn::hid::KEY_ZR)) {
			if(btnDown(CLEAR_TCPLOG, p2Cur.Buttons, p2Prev.Buttons)) {
				// https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#text-modification
				skyline::logger::s_Instance->LogFormat("\u001B[2J");

				bf2mods::g_Logger->LogInfo("Cleared TCP log");
				bf2mods::PlaySE(gf::GfMenuObjUtil::SEIndex::Sort);
			}

			if(btnDown(BDAT_SCRAMBLETYPE_TOGGLE, p2Cur.Buttons, p2Prev.Buttons)) {
				bf2mods::underlying_value(sharedStatePtr->options.bdatScrambleType) += 1;

				if(sharedStatePtr->options.bdatScrambleType >= bf2mods::Options::BdatScrambleType::Count)
					sharedStatePtr->options.bdatScrambleType = bf2mods::Options::BdatScrambleType::Off;

				bf2mods::g_Logger->LogInfo("Bdat scramble type set to %s", bf2mods::format(sharedStatePtr->options.bdatScrambleType).c_str());
			}

			if(btnDown(MOVEMENT_SPEED_UP, p2Cur.Buttons, p2Prev.Buttons)) {
				sharedStatePtr->options.movementSpeedMult *= 2.0f;
				bf2mods::g_Logger->LogInfo("Movement speed multiplier set to %.2f", sharedStatePtr->options.movementSpeedMult);
			}

			if(btnDown(MOVEMENT_SPEED_DOWN, p2Cur.Buttons, p2Prev.Buttons)) {
				sharedStatePtr->options.movementSpeedMult *= 0.5f;
				bf2mods::g_Logger->LogInfo("Movement speed multiplier set to %.2f", sharedStatePtr->options.movementSpeedMult);
			}

			if(btnDown(DISABLE_FALL_DAMAGE, p2Cur.Buttons, p2Prev.Buttons)) {
				sharedStatePtr->options.disableFallDamage = !sharedStatePtr->options.disableFallDamage;
				bf2mods::g_Logger->LogInfo("Disable fall damage: %s", bf2mods::format(sharedStatePtr->options.disableFallDamage).c_str());
			}

			/*if (btnDown(LAYER_OPEN, p2Cur.Buttons, p2Prev.Buttons)) {
				bf2mods::g_Logger->LogInfo("Opening layer %u", bf2mods::Plugin::getSharedStatePtr()->mapjumpId);
				bf2mods::OpenLayer(bf2mods::Plugin::getSharedStatePtr()->mapjumpId);
			}
			if (btnDown(LAYER_CLOSE, p2Cur.Buttons, p2Prev.Buttons)) {
				bf2mods::g_Logger->LogInfo("Closing layer %u", bf2mods::Plugin::getSharedStatePtr()->mapjumpId);
				bf2mods::CloseLayer(bf2mods::Plugin::getSharedStatePtr()->mapjumpId);
			}*/

			if(btnDown(FREECAM_TOGGLE, p2Cur.Buttons, p2Prev.Buttons)) {
				sharedStatePtr->freecam.isOn = !sharedStatePtr->freecam.isOn;
				bf2mods::g_Logger->LogInfo("Toggling freecam: %s", bf2mods::format(sharedStatePtr->freecam.isOn).c_str());
			}
		} else {
			if(btnDown(MAPJUMP_INC, p2Cur.Buttons, p2Prev.Buttons)) {
				sharedStatePtr->mapjumpId++;
				if(sharedStatePtr->mapjumpId >= 298)
					sharedStatePtr->mapjumpId = 1;

				bf2mods::g_Logger->LogInfo("MapJump++, now %d", sharedStatePtr->mapjumpId);
			}
			if(btnDown(MAPJUMP_DEC, p2Cur.Buttons, p2Prev.Buttons)) {
				sharedStatePtr->mapjumpId--;
				if(sharedStatePtr->mapjumpId <= 0)
					sharedStatePtr->mapjumpId = 297;

				bf2mods::g_Logger->LogInfo("MapJump--, now %d", sharedStatePtr->mapjumpId);
			}

			if(btnDown(MAPJUMP_JUMP, p2Cur.Buttons, p2Prev.Buttons)) {
				bf2mods::g_Logger->LogInfo("Attempting jump to MapJump %d", sharedStatePtr->mapjumpId);
				bf2mods::DoMapJump(sharedStatePtr->mapjumpId);
			}

			if(btnDown(PLAYSE, p2Cur.Buttons, p2Prev.Buttons)) {
				bf2mods::g_Logger->LogInfo("Sound effect %d (0x%x)", sharedStatePtr->mapjumpId, sharedStatePtr->mapjumpId);
				bf2mods::PlaySE(sharedStatePtr->mapjumpId);
			}

			if(btnDown(RETURN_TO_TITLE, p2Cur.Buttons, p2Prev.Buttons)) {
				bf2mods::PlaySE(gf::GfMenuObjUtil::SEIndex::Sort);
				bf2mods::ReturnTitle(-1);
			}
		}

		p1Prev = p1Cur;
		p2Prev = p2Cur;

		// draw log messages
		bf2mods::g_Logger->Draw();
	}

} // namespace fw

namespace bf2mods {

	HidInput p1Cur {};
	HidInput p1Prev {};
	HidInput p2Cur {};
	HidInput p2Prev {};

	void bf2mods_main() {
		Plugin::init();

		// hook stuff
		SetupDebugStuff();
		SetupPlayerMovementHooks();
		SetupBdatRandomizer();
		SetupMenuViewer();
		CameraTools::SetupCameraTools();

		// for debug keys
		fw::Framework_updateHook();
	}

} // namespace bf2mods
