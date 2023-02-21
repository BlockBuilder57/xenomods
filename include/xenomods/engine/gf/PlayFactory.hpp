//
// Created by block on 7/31/2022.
//

#pragma once

namespace gf {

	struct GfSequentialPlaySignal {};

	enum class MAINMENU {
		Main,
		System,
		SkipTravel
	};

	namespace GfPlayFactory {

		bool createOpenMenu(unsigned int type, GfSequentialPlaySignal* signal);
		bool createOpenMenu2(unsigned int type, MAINMENU menu, GfSequentialPlaySignal* signal);

		void createSkipTravel(unsigned int mapjumpId);
		char* getEventName(unsigned int eventId);
		unsigned int getEventID(const char* evtName);
	};

}