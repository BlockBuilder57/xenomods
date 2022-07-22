#include "plugin_main.hpp"

#include <bf2mods/mm/math_types.hpp>
#include <bf2mods/fw/debug.hpp>
#include <bf2mods/fw/document.hpp>
#include <bf2mods/stuff/utils/debug_util.hpp>

#include <version.h>
#include "bf2logger.hpp"
#include "state.hpp"
#include "skyline/efl/service.hpp"

// all parts here
#include "bdat_randomizer.hpp"
#include "camera_tools.hpp"
#include "debug_stuff.hpp"
#include "menu_viewer.hpp"
#include "player_movement.hpp"

namespace fw {

	Document* document;

	GENERATE_SYM_HOOK(Framework_update, "_ZN2fw9Framework6updateEv", void, void* that) {
		Framework_updateBak(that);
		bf2mods::update();
	}

	GENERATE_SYM_HOOK(FrameworkUpdater_updateStd, "_ZN2fw16FrameworkUpdater9updateStdERKNS_8DocumentEPNS_19FrameworkControllerE", void, fw::Document* doc, void* FrameworkController) {
		FrameworkUpdater_updateStdBak(doc, FrameworkController);

		if (doc != nullptr && document == nullptr) {
			bf2mods::g_Logger->LogInfo("got valid document ptr %p", doc);
			document = doc;
		}

		bf2mods::update();
	}

} // namespace fw

namespace bf2mods {

	HidInput p1Cur {};
	HidInput p1Prev {};
	HidInput p2Cur {};
	HidInput p2Prev {};

	void update() {
		// lazy
		using enum bf2mods::Keybind;

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
		//fw::debug::drawFont(1280-buttonsP1Width-5, 5, mm::Col4::White, "%xh - P1", p1Cur.Buttons);
		//fw::debug::drawFont(1280-buttonsP2Width-5, 5+16, mm::Col4::White, "%xh - P2", p2Cur.Buttons);

		/*
		 * Enforce some things on first update
		 */
		static bool hasUpdated;
		if(!hasUpdated) {
			nn::hid::SetSupportedNpadStyleSet(3);
#if !BF2MODS_CODENAME(bfsw)
			fw::PadManager::enableDebugDraw(true);
#endif
			hasUpdated = true;
		}

		/*
		 * Check buttons
		 */

		auto& state = GetState();

		state.moonJump = btnHeld(MOONJUMP, p1Cur.Buttons);

		if(state.freecam.isOn)
			CameraTools::DoFreeCameraMovement();

		bf2mods::DebugStuff::bgmTrackIndex = 0;

		if(!(p2Cur.Buttons & nn::hid::KEY_ZL) && !(p2Cur.Buttons & nn::hid::KEY_ZR)) {
			if(btnDown(CLEAR_TCPLOG, p2Cur.Buttons, p2Prev.Buttons)) {
				// https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#text-modification
				skyline::logger::s_Instance->LogFormat("\u001B[2J");

				g_Logger->LogInfo("Cleared TCP log");
				DebugStuff::PlaySE(gf::GfMenuObjUtil::SEIndex::Sort);
			}

			if(btnDown(BDAT_SCRAMBLETYPE_TOGGLE, p2Cur.Buttons, p2Prev.Buttons)) {
				underlying_value(state.options.bdatScrambleType) += 1;

				if(state.options.bdatScrambleType >= bf2mods::Options::BdatScrambleType::Count)
					state.options.bdatScrambleType = bf2mods::Options::BdatScrambleType::Off;

				g_Logger->LogInfo("Bdat scramble type set to %s", bf2mods::format(state.options.bdatScrambleType).c_str());
			}

			if(btnDown(MOVEMENT_SPEED_UP, p2Cur.Buttons, p2Prev.Buttons)) {
				state.options.movementSpeedMult *= 2.0f;
				g_Logger->LogInfo("Movement speed multiplier set to %.2f", state.options.movementSpeedMult);
			}

			if(btnDown(MOVEMENT_SPEED_DOWN, p2Cur.Buttons, p2Prev.Buttons)) {
				state.options.movementSpeedMult *= 0.5f;
				g_Logger->LogInfo("Movement speed multiplier set to %.2f", state.options.movementSpeedMult);
			}

			if(btnDown(DISABLE_FALL_DAMAGE, p2Cur.Buttons, p2Prev.Buttons)) {
				state.options.disableFallDamage = !state.options.disableFallDamage;
				g_Logger->LogInfo("Disable fall damage: %s", bf2mods::format(state.options.disableFallDamage).c_str());
			}

			if(btnDown(FREECAM_TOGGLE, p2Cur.Buttons, p2Prev.Buttons)) {
				state.freecam.isOn = !state.freecam.isOn;
				g_Logger->LogInfo("Toggling freecam: %s", bf2mods::format(state.freecam.isOn).c_str());
			}

			if(btnDown(UI_TOGGLE, p2Cur.Buttons, p2Prev.Buttons)) {
				state.options.enableUIRendering = !state.options.enableUIRendering;
				g_Logger->LogInfo("UI rendering: %s", bf2mods::format(state.options.enableUIRendering).c_str());
			}
		} else {
			if(btnDown(MAPJUMP_INC, p2Cur.Buttons, p2Prev.Buttons)) {
				state.mapjumpId++;
				if(state.mapjumpId >= 298)
					state.mapjumpId = 1;

				g_Logger->LogInfo("MapJump++, now %d", state.mapjumpId);
			}
			if(btnDown(MAPJUMP_DEC, p2Cur.Buttons, p2Prev.Buttons)) {
				state.mapjumpId--;
				if(state.mapjumpId <= 0)
					state.mapjumpId = 297;

				g_Logger->LogInfo("MapJump--, now %d", state.mapjumpId);
			}

			if(btnDown(MAPJUMP_JUMP, p2Cur.Buttons, p2Prev.Buttons)) {
				g_Logger->LogInfo("Attempting jump to MapJump %d", state.mapjumpId);
				DebugStuff::DoMapJump(state.mapjumpId);
			}

			if(btnDown(PLAYSE, p2Cur.Buttons, p2Prev.Buttons)) {
				g_Logger->LogInfo("Sound effect %d (0x%x)", state.mapjumpId, state.mapjumpId);
				DebugStuff::PlaySE(state.mapjumpId);
			}

			if(btnDown(RETURN_TO_TITLE, p2Cur.Buttons, p2Prev.Buttons)) {
				DebugStuff::PlaySE(gf::GfMenuObjUtil::SEIndex::Sort);
				DebugStuff::ReturnTitle(-1);
			}
		}

		p1Prev = p1Cur;
		p2Prev = p2Cur;

		// draw log messages
		g_Logger->Draw();
	}

	void bf2mods_main() {
		g_Logger->LogInfo("bf2mods - %s (built %s %s)", version::tagDirty, __DATE__, __TIME__);

		// hook stuff
		DebugStuff::Setup();
		BdatRandomizer::Setup();
		CameraTools::Setup();
		MenuViewer::Setup();

		PlayerMovement::Setup();
#if BF2MODS_CODENAME(bf2) || BF2MODS_CODENAME(ira)
#endif

		// for debug keys
#if !BF2MODS_CODENAME(bfsw)
		fw::Framework_updateHook();
#else
		fw::FrameworkUpdater_updateStdHook();
#endif
	}

} // namespace bf2mods
