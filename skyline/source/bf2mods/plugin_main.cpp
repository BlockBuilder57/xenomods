#include "bf2logger.hpp"
#include "plugin.hpp"
#include "skyline/efl/service.hpp"

// all parts here
#include "debug_stuff.hpp"
#include "player_movement.hpp"
#include "bdat_randomizer.hpp"
#include "test.hpp"

#include <bf2mods/mm/math_types.hpp>

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

		static constexpr auto KEY_PLAYSE = nn::hid::KEY_MINUS | nn::hid::KEY_ZL;

		static constexpr auto KEY_MOONJUMP = nn::hid::KEY_R | nn::hid::KEY_B;
		static constexpr auto KEY_DISABLE_FALL_DAMAGE = nn::hid::KEY_A;

		static constexpr auto KEY_RETURN_TO_TITLE = nn::hid::KEY_L | nn::hid::KEY_R | nn::hid::KEY_ZL | nn::hid::KEY_ZR;

		auto btnHeld = [](std::uint64_t combo, std::uint64_t buttons) -> bool {
			return (buttons & combo) == combo;
		};
		auto btnUp = [](std::uint64_t combo, std::uint64_t curButtons, std::uint64_t prevButtons) -> bool {
			return (curButtons & combo) == combo && (prevButtons & combo) != combo;
		};
		auto btnDown = [](std::uint64_t combo, std::uint64_t curButtons, std::uint64_t prevButtons) -> bool {
			return (curButtons & combo) != combo && (prevButtons & combo) == combo;
		};

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


		//int buttonsP1Width = fw::debug::drawFontGetWidth("%xh - P1", curP1Buttons);
		//int buttonsP2Width = fw::debug::drawFontGetWidth("%xh - P2", curP2Buttons);
		//fw::debug::drawFont(1280-buttonsP1Width-5, 5, &mm::Col4::White, "%xh - P1", curP1Buttons);
		//fw::debug::drawFont(1280-buttonsP2Width-5, 5+16, &mm::Col4::White, "%xh - P2", curP2Buttons);


		/*
		 * Check buttons
		 */

		bf2mods::Plugin::getSharedStatePtr()->moonJump = btnHeld(KEY_MOONJUMP, curP1Buttons);

		if (!(curP2Buttons & nn::hid::KEY_ZL) && !(curP2Buttons & nn::hid::KEY_ZR)) {
			if(btnDown(KEY_CLEAR_TCPLOG, curP2Buttons, prevP2Buttons)) {
				// https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#text-modification
				skyline::logger::s_Instance->LogFormat("\u001B[2J");

				bf2mods::g_Logger->LogInfo("Cleared TCP log");
				bf2mods::PlaySE(gf::GfMenuObjUtil::SEIndex::Sort);
			}

			if(btnDown(KEY_BDAT_SCRAMBLETYPE_TOGGLE, curP2Buttons, prevP2Buttons)) {
				reinterpret_cast<std::underlying_type_t<decltype(bf2mods::Plugin::getSharedStatePtr()->options.bdat.scrambleType)>&>(bf2mods::Plugin::getSharedStatePtr()->options.bdat.scrambleType) += 1;

				if (bf2mods::Plugin::getSharedStatePtr()->options.bdat.scrambleType >= bf2mods::SharedState::Options::BdatOptions::ScrambleType::Count )
					bf2mods::Plugin::getSharedStatePtr()->options.bdat.scrambleType = bf2mods::SharedState::Options::BdatOptions::ScrambleType::Off;

				bf2mods::g_Logger->LogInfo("Bdat scramble type set to %s", bf2mods::format(bf2mods::Plugin::getSharedStatePtr()->options.bdat.scrambleType).c_str());
			}

			if(btnDown(KEY_MOVEMENT_SPEED_UP, curP2Buttons, prevP2Buttons)) {
				bf2mods::Plugin::getSharedStatePtr()->options.game.movementSpeedMult *= 2.0f;
				bf2mods::g_Logger->LogInfo("Movement speed multiplier set to %.2f", bf2mods::Plugin::getSharedStatePtr()->options.game.movementSpeedMult);
			}
			if(btnDown(KEY_MOVEMENT_SPEED_DOWN, curP2Buttons, prevP2Buttons)) {
				bf2mods::Plugin::getSharedStatePtr()->options.game.movementSpeedMult *= 0.5f;
				bf2mods::g_Logger->LogInfo("Movement speed multiplier set to %.2f", bf2mods::Plugin::getSharedStatePtr()->options.game.movementSpeedMult);
			}

			if (btnDown(KEY_DISABLE_FALL_DAMAGE, curP2Buttons, prevP2Buttons)) {
				bf2mods::Plugin::getSharedStatePtr()->options.game.disableFallDamage = !bf2mods::Plugin::getSharedStatePtr()->options.game.disableFallDamage;
				bf2mods::g_Logger->LogInfo("Disable fall damage: %s", bf2mods::format(bf2mods::Plugin::getSharedStatePtr()->options.game.disableFallDamage).c_str());
			}
		}
		else {
			if(btnDown(KEY_MAPJUMP_INC, curP2Buttons, prevP2Buttons)) {
				bf2mods::Plugin::getSharedStatePtr()->mapjumpId++;
				bf2mods::g_Logger->LogInfo("MapJump++, now %d", bf2mods::Plugin::getSharedStatePtr()->mapjumpId);
			}
			if(btnDown(KEY_MAPJUMP_DEC, curP2Buttons, prevP2Buttons)) {
				bf2mods::Plugin::getSharedStatePtr()->mapjumpId--;
				if (bf2mods::Plugin::getSharedStatePtr()->mapjumpId <= 0)
					bf2mods::Plugin::getSharedStatePtr()->mapjumpId = 1;

				bf2mods::g_Logger->LogInfo("MapJump--, now %d", bf2mods::Plugin::getSharedStatePtr()->mapjumpId);
			}

			if(btnDown(KEY_MAPJUMP_JUMP, curP2Buttons, prevP2Buttons)) {
				bf2mods::g_Logger->LogInfo("Attempting jump to MapJump %d", bf2mods::Plugin::getSharedStatePtr()->mapjumpId);
				bf2mods::DoMapJump(bf2mods::Plugin::getSharedStatePtr()->mapjumpId);
			}

			if(btnDown(KEY_PLAYSE, curP2Buttons, prevP2Buttons)) {
				bf2mods::g_Logger->LogInfo("Sound effect %d (0x%x)", bf2mods::Plugin::getSharedStatePtr()->mapjumpId, bf2mods::Plugin::getSharedStatePtr()->mapjumpId);
				bf2mods::PlaySE(bf2mods::Plugin::getSharedStatePtr()->mapjumpId);
			}

			if (btnDown(KEY_RETURN_TO_TITLE, curP2Buttons, prevP2Buttons)) {
				bf2mods::PlaySE(gf::GfMenuObjUtil::SEIndex::Sort);
				bf2mods::ReturnTitle(-1);
			}
		}


		prevP1Buttons = curP1Buttons;
		prevP2Buttons = curP2Buttons;

		// draw log messages
		bf2mods::g_Logger->Draw();
	}

}

namespace bf2mods {

	void SetupEverything() {
		Plugin::init();

		// hook stuff
		SetupDebugStuff();
		SetupPlayerMovementHooks();
		SetupBdatRandomizer();

		// lazy
		testhook();

		// for debug keys
		fw::Framework_updateHook();
	}

} // namespace bf2mods
