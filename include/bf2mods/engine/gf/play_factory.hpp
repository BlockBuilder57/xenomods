//
// Created by block on 7/31/2022.
//

#pragma once

namespace gf {

	namespace GfPlayFactory {
		void createSkipTravel(unsigned int mapjumpId);
		char* getEventName(unsigned int eventId);
		unsigned int getEventID(const char* evtName);
	};

}