#include "bf2logger.h"
#include "plugin.hpp"
#include "skyline/efl/service.hpp"

// all parts here
#include "bdat_randomizer.hpp"
#include "debug_stuff.hpp"
#include "test.hpp"
#include <bf2mods/mm/math_types.hpp>

namespace bf2mods {

	namespace fw {

		GENERATE_SYM_HOOK(Framework_update, "_ZN2fw9Framework6updateEv", void, void* that) {
			Framework_updateBak(that);

			static constexpr auto KEY_CLEAR_TCPLOG = nn::hid::KEY_RSTICK;
			static constexpr auto KEY_BDAT_SCRAMBLETYPE_TOGGLE = nn::hid::KEY_X;

			static constexpr auto KEY_MOVEMENT_SPEED_UP = nn::hid::KEY_DUP;
			static constexpr auto KEY_MOVEMENT_SPEED_DOWN = nn::hid::KEY_DDOWN;

			static constexpr auto KEY_MAPJUMP_INC = nn::hid::KEY_DRIGHT | nn::hid::KEY_ZL;
			static constexpr auto KEY_MAPJUMP_DEC = nn::hid::KEY_DLEFT | nn::hid::KEY_ZL;
			static constexpr auto KEY_MAPJUMP_JUMP = nn::hid::KEY_DDOWN | nn::hid::KEY_ZL;

			static constexpr auto KEY_TEST_PLAYSE = nn::hid::KEY_MINUS | nn::hid::KEY_ZL;

			static constexpr auto KEY_MOONJUMP = nn::hid::KEY_R | nn::hid::KEY_B;

			/*
			 * Scan HIDs
			 */

			std::uint64_t curP1Buttons = 0ul;
			static std::uint64_t prevP1Buttons = 0ul;
			std::uint64_t curP2Buttons = 0ul;
			static std::uint64_t prevP2Buttons = 0ul;

			auto npadP1FullKeyState = nn::hid::NpadFullKeyState {};
			nn::hid::GetNpadState(&npadP1FullKeyState, nn::hid::CONTROLLER_PLAYER_1);
			curP1Buttons |= npadP1FullKeyState.Buttons;
			auto npadHandheldState = nn::hid::NpadHandheldState {};
			nn::hid::GetNpadState(&npadHandheldState, nn::hid::CONTROLLER_HANDHELD);
			curP1Buttons |= npadHandheldState.Buttons;

			auto npadP2FullKeyState = nn::hid::NpadFullKeyState {};
			nn::hid::GetNpadState(&npadP2FullKeyState, nn::hid::CONTROLLER_PLAYER_2);
			curP2Buttons |= npadP2FullKeyState.Buttons;


			//if (curP2Buttons != prevP2Buttons && curP2Buttons != 0)
			//	skyline::logger::s_Instance->LogFormat("BUTTONS CHANGED: 0x%16x", curP2Buttons);
			//fw::debug::drawFont(5, 5, &mm::Col4_White, "BUTTONS: 0x%16x", curP2Buttons);


			/*
			 * Check buttons
			 */

			Plugin::getSharedStatePtr()->moonJump = curP1Buttons & KEY_MOONJUMP;

			if (!(curP2Buttons & nn::hid::KEY_ZL) && !(curP2Buttons & nn::hid::KEY_ZR)) {
				if(curP2Buttons == KEY_CLEAR_TCPLOG && !(prevP2Buttons == KEY_CLEAR_TCPLOG)) {
					// https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#text-modification
					skyline::logger::s_Instance->LogFormat("\u001B[2J");

					g_Logger->LogInfo("Cleared TCP log");
					PlaySE(gf::GfMenuObjUtil::SEIndex::Sort);
				}

				if(curP2Buttons == KEY_BDAT_SCRAMBLETYPE_TOGGLE && !(prevP2Buttons == KEY_BDAT_SCRAMBLETYPE_TOGGLE)) {
					reinterpret_cast<std::underlying_type_t<decltype(Plugin::getSharedStatePtr()->options.bdat.scrambleType)>&>(Plugin::getSharedStatePtr()->options.bdat.scrambleType) += 1;

					if (Plugin::getSharedStatePtr()->options.bdat.scrambleType >= SharedState::Options::BdatOptions::ScrambleType::Count )
						Plugin::getSharedStatePtr()->options.bdat.scrambleType = SharedState::Options::BdatOptions::ScrambleType::Off;

					g_Logger->LogInfo("Bdat scramble type set to %s", bf2mods::format(Plugin::getSharedStatePtr()->options.bdat.scrambleType).c_str());
				}

				if(curP2Buttons == KEY_MOVEMENT_SPEED_UP && !(prevP2Buttons == KEY_MOVEMENT_SPEED_UP)) {
					Plugin::getSharedStatePtr()->options.game.movementSpeedMult *= 2.0f;
					g_Logger->LogInfo("Movement speed multiplier set to %.2f", Plugin::getSharedStatePtr()->options.game.movementSpeedMult);
				}
				if(curP2Buttons == KEY_MOVEMENT_SPEED_DOWN && !(prevP2Buttons == KEY_MOVEMENT_SPEED_DOWN)) {
					Plugin::getSharedStatePtr()->options.game.movementSpeedMult *= 0.5f;
					g_Logger->LogInfo("Movement speed multiplier set to %.2f", Plugin::getSharedStatePtr()->options.game.movementSpeedMult);
				}
			}
			else {
				if(curP2Buttons == KEY_MAPJUMP_INC && !(prevP2Buttons == KEY_MAPJUMP_INC)) {
					Plugin::getSharedStatePtr()->mapjumpId++;
					g_Logger->LogInfo("MapJump++, now %d", Plugin::getSharedStatePtr()->mapjumpId);
				}
				if(curP2Buttons == KEY_MAPJUMP_DEC && !(prevP2Buttons == KEY_MAPJUMP_DEC)) {
					Plugin::getSharedStatePtr()->mapjumpId--;
					if (Plugin::getSharedStatePtr()->mapjumpId <= 0)
						Plugin::getSharedStatePtr()->mapjumpId = 1;

					g_Logger->LogInfo("MapJump--, now %d", Plugin::getSharedStatePtr()->mapjumpId);
				}

				if(curP2Buttons == KEY_MAPJUMP_JUMP && !(prevP2Buttons == KEY_MAPJUMP_JUMP)) {
					g_Logger->LogInfo("Attempting jump to MapJump %d", Plugin::getSharedStatePtr()->mapjumpId);
					DoMapJump(Plugin::getSharedStatePtr()->mapjumpId);
				}

				if(curP2Buttons == KEY_TEST_PLAYSE && !(prevP2Buttons == KEY_TEST_PLAYSE)) {
					g_Logger->LogInfo("Sound effect %d (0x%x)", Plugin::getSharedStatePtr()->mapjumpId, Plugin::getSharedStatePtr()->mapjumpId);
					PlaySE(Plugin::getSharedStatePtr()->mapjumpId);
				}
			}

			prevP1Buttons = curP1Buttons;
			prevP2Buttons = curP2Buttons;

			// draw log messages
			g_Logger->Draw();
		}

	}

	void SetupEverything() {
		Plugin::init();

		// hook stuff
		SetupBdatRandomizer();
		SetupDebugStuff();

		// lazy
		testhook();

		// for debug keys
		fw::Framework_updateHook();


	}

} // namespace bf2mods
