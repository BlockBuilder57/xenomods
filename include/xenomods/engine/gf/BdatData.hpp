//
// Created by block on 7/31/2022.
//

#pragma once

namespace gf {

	namespace GfDataEvent {
		static char* getEventName(unsigned int eventId);
		static unsigned int getEventID(const char* evtName);
		static bool isWatchedEventFlag(unsigned int eventId);
	};

	namespace GfDataBdat {
		static unsigned char* getFP(unsigned int idx);
	};

}