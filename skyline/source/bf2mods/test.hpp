#pragma once

// This file is entirely by 3096 with only minor touchups.

#include <map>

#include "bf2mods/stuff/utils/debug_util.hpp"

namespace ml {

	struct EventView { /* PlaceHolder Structure */
		char field_0x0;
		char field_0x1;
		char field_0x2;
		char field_0x3;
		char field_0x4;
		char field_0x5;
		char field_0x6;
		char field_0x7;
		char field_0x8;
		char field_0x9;
		char field_0xa;
		char field_0xb;
		char field_0xc;
		char field_0xd;
		char field_0xe;
		char field_0xf;
		int mousePointX;
		int mousePointY;
		uint field_0x18;
		char field_0x1c;
		char field_0x1d;
		char field_0x1e;
		char field_0x1f;
		char field_0x20;
		char field_0x21;
		char field_0x22;
		char field_0x23;
		uint mouseButtonField;
		uint bitField;
		char keyboardIsPressed; /* Created by retype action */
		char field_0x2d;
		char field_0x2e;
		char field_0x2f;
		char field_0x30;
		char field_0x31;
		char field_0x32;
		char keyId;
	};

} // namespace ml

namespace event {

	class CamFree {
	   public:
		void* vtable_0x0;
		char field_0x8;
		char field_0x9;
		char field_0xa;
		char field_0xb;
		char field_0xc;
		char field_0xd;
		char field_0xe;
		char field_0xf;
		mm::Vec3 camPos_0x10;
		char field_0x1c;
		char field_0x1d;
		char field_0x1e;
		char field_0x1f;
		mm::Vec3 lookAtPos_0x20;
		char field_0x2c;
		char field_0x2d;
		char field_0x2e;
		char field_0x2f;
		float field_0x30;
		float field_0x34;
		int32_t field_0x38;
		int32_t field_0x3c;
		int64_t p_workEvent_0x40;
		char field_0x48;
		char field_0x49;
		char field_0x4a;
		char field_0x4b;
		uint curDownKeyTypeBits_0x4c;
		uint32_t curMouseKey_0x50;
		float field_0x54;

		bool OnKeyDown(ml::EventView*);
		bool OnMouseButtonDown(ml::EventView*);
		bool OnMouseButtonUp(ml::EventView*);
		bool OnMouseMove(ml::EventView*);
		void setActive(bool);
	};
	static_assert(sizeof(CamFree) == 0x58, "size 0x58");

	class CamManager {
	   public:
		bool isFreeCam() const;
		void setFreeCam(bool);
		CamFree* getFreeCam();
	};

	class Manager {
	   public:
		void drawInfo();
		void setSyncFreeCam(bool, bool);
	};

} // namespace event

GENERATE_SYM_HOOK(eventManagerUpdate, "_ZN5event7Manager6updateEv", void, event::Manager* p_this) {
	// dbgutil::poorPersonsBreakpoint("eventManagerUpdate");
	// static auto bruh = false;
	// if (not bruh) {
	//     bruh = true;
	// }
	p_this->drawInfo();
	// p_this->setSyncFreeCam(true, true);
	return eventManagerUpdateBak(p_this);
}

GENERATE_SYM_HOOK(mlDevKeyboardGetVkHold, "_ZN2ml11DevKeyboard9getVkHoldEv", int) {
	return 1;
}

float m_movementMult = 1.0f;

GENERATE_SYM_HOOK(eventCamManUpdate, "_ZN5event10CamManager6updateEv", size_t, event::CamManager* p_this) {
	static constexpr auto KEY_TOGGLE = nn::hid::KEY_MINUS;

	static constexpr auto KEY_FOV_INCREASE = nn::hid::KEY_LSTICK;
	static constexpr auto KEY_FOV_DECREASE = nn::hid::KEY_RSTICK;
	static constexpr auto KEY_ZOOM_IN = nn::hid::KEY_X;
	static constexpr auto KEY_ZOOM_OUT = nn::hid::KEY_Y;
	// static constexpr auto KEY_RISE = nn::hid::KEY_ZR;
	// static constexpr auto KEY_SINK = nn::hid::KEY_ZL;
	// static constexpr auto KEY_STRAFE_LEFT = nn::hid::KEY_L;
	// static constexpr auto KEY_STRAFE_RIGHT = nn::hid::KEY_R;
	static constexpr auto KEY_SPIN_LEFT = nn::hid::KEY_DLEFT;
	static constexpr auto KEY_SPIN_RIGHT = nn::hid::KEY_DRIGHT;
	static constexpr auto KEY_SPEED_INCREASE = nn::hid::KEY_DUP;
	static constexpr auto KEY_SPEED_DECREASE = nn::hid::KEY_DDOWN;

	static const auto PAD_KEY_MAP = std::map<decltype(nn::hid::NpadFullKeyState::Buttons), char> {
		{ KEY_FOV_INCREASE, 0x25 },
		{ KEY_ZOOM_IN, 0x26 },
		{ KEY_FOV_DECREASE, 0x27 },
		{ KEY_ZOOM_OUT, 0x28 },
		{ KEY_SPIN_RIGHT, 0x81 },
		// {KEY_SINK, 0x82},
		{ KEY_SPIN_LEFT, 0x83 },
		// {KEY_STRAFE_LEFT, 0x84},
		// {KEY_STRAFE_RIGHT, 0x86},
		// {KEY_RISE, 0x88},
	};

	static constexpr auto KEY_FORWARD = nn::hid::KEY_R;
	static constexpr auto KEY_BACK = nn::hid::KEY_L;

	float DELTA_UNIT = 0.1;

	// scan hid
	auto npadFullKeyState = nn::hid::NpadFullKeyState {};
	static auto prevButtons = decltype(npadFullKeyState.Buttons) {};
	nn::hid::GetNpadState(&npadFullKeyState, nn::hid::CONTROLLER_PLAYER_1);
	auto curButtons = npadFullKeyState.Buttons;
	auto npadHandheldState = nn::hid::NpadHandheldState {};
	nn::hid::GetNpadState(&npadHandheldState, nn::hid::CONTROLLER_HANDHELD);
	curButtons |= npadHandheldState.Buttons;

	if(curButtons & KEY_SPEED_INCREASE && !(prevButtons & KEY_SPEED_INCREASE))
		m_movementMult *= 2.0f;
	else if(curButtons & KEY_SPEED_DECREASE && !(prevButtons & KEY_SPEED_DECREASE))
		m_movementMult *= 0.5f;

	// check toggle
	auto curIsFreeCam = p_this->isFreeCam();
	if(curButtons == KEY_TOGGLE and prevButtons != KEY_TOGGLE) {
		curIsFreeCam = !curIsFreeCam;
		p_this->setFreeCam(curIsFreeCam);
	}
	prevButtons = curButtons;

	if(not curIsFreeCam) {
		return eventCamManUpdateBak(p_this);
	}

	// update position
	auto p_freeCam = p_this->getFreeCam();

	auto delta = p_freeCam->lookAtPos_0x20 - p_freeCam->camPos_0x10;
	if(curButtons & KEY_FORWARD) {
		p_freeCam->camPos_0x10 += delta * (DELTA_UNIT * m_movementMult);
		p_freeCam->lookAtPos_0x20 += delta * (DELTA_UNIT * m_movementMult);
	} else if(curButtons & KEY_BACK) {
		p_freeCam->camPos_0x10 -= delta * (DELTA_UNIT * m_movementMult);
		p_freeCam->lookAtPos_0x20 -= delta * (DELTA_UNIT * m_movementMult);
	}

	// handle key events
	auto fakeEventView = ml::EventView {};
	fakeEventView.bitField = -1; // not sure what this does yet, but simply flipping it on seems to do the trick

	for(auto& entry : PAD_KEY_MAP) {
		if((curButtons | entry.first) == curButtons) {
			fakeEventView.keyboardIsPressed = 1;
			fakeEventView.keyId = entry.second;
			break;
		}
	}
	p_freeCam->OnKeyDown(&fakeEventView);

	// release all mouse buttons first
	fakeEventView.mouseButtonField = -1;
	p_freeCam->OnMouseButtonUp(&fakeEventView);

	// rotate
	fakeEventView.mouseButtonField = 1;
	fakeEventView.mousePointX = npadFullKeyState.RStickX / 5000;
	fakeEventView.mousePointY = -npadFullKeyState.RStickY / 5000;
	p_freeCam->OnMouseButtonDown(&fakeEventView);
	p_freeCam->OnMouseMove(&fakeEventView);

	// move
	fakeEventView.mouseButtonField = 2;
	fakeEventView.mousePointX = -npadFullKeyState.LStickX / 5000;
	fakeEventView.mousePointY = npadFullKeyState.LStickY / 5000;
	p_freeCam->OnMouseButtonDown(&fakeEventView);
	p_freeCam->OnMouseMove(&fakeEventView);

	return eventCamManUpdateBak(p_this);
}

GENERATE_SYM_HOOK(eventCamFreeUpdate, "_ZN5event7CamFree6updateERKNS_9FrameInfoE", void, void* p_this) {
	dbgutil::poorPersonsBreakpoint("eventCamFreeUpdate");
	return eventCamFreeUpdateBak(p_this);
}

GENERATE_SYM_HOOK(eventCamFreeOnKeyDown, "_ZN5event10BouManager17OnMouseButtonDownEPN2ml9EventViewE", int32_t,
				  void* p_this, void* p_eventView) {
	dbgutil::poorPersonsBreakpoint("eventCamFreeOnKeyDown");
	return eventCamFreeOnKeyDownBak(p_this, p_eventView);
}

GENERATE_SYM_HOOK(eventManagerOnKeyDown, "_ZN5event7Manager9OnKeyDownEPN2ml9EventViewE", int32_t,
				  event::Manager* p_this, void* p_eventView) {
	dbgutil::poorPersonsBreakpoint("eventManagerOnKeyDown");
	return 0;
}

GENERATE_SYM_HOOK(getOperationMode, "_ZN2nn2oe16GetOperationModeEv", int) {
	return 1;
}

GENERATE_SYM_HOOK(getPerformanceMode, "_ZN2nn2oe18GetPerformanceModeEv", int) {
	return 1;
}

GENERATE_SYM_HOOK(setAutoReduction, "_ZN2ml5DrMan16setAutoReductionEb", u64, void* p_this, bool value) {
	return setAutoReductionBak(p_this, false);
}

void logX(InlineCtx* ctx) {
	dbgutil::logRegistersX(ctx);
	dbgutil::poorPersonsBreakpoint("log x");
}

void testhook() {
	// eventManagerUpdateHook();
	mlDevKeyboardGetVkHoldHook();
	eventCamManUpdateHook();
	eventCamFreeUpdateHook();
	eventCamFreeOnKeyDownHook();
	eventManagerOnKeyDownHook();

	getOperationModeHook();
	getPerformanceModeHook();
	setAutoReductionHook();
}
