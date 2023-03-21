//
// Created by block on 7/31/2022.
//

#pragma once

namespace gf {

	class GfDataEvent {
	   public:
		static char* getEventName(unsigned int eventId);
		static unsigned int getEventID(const char* evtName);
		static bool isWatchedEventFlag(unsigned int eventId);
	};

	class GfDataBdat {
	   public:
		static unsigned char* getFP(unsigned int idx);
	};

}