//
// Created by block on 7/31/2022.
//

#pragma once

#include "xenomods/engine/ui/UIStr.hpp"

namespace gf {

	class GfBdatMsg {
	   public:
		static char* getNameMapInfo(unsigned int);
	};

	class GfDataMap {
	   public:
		static char* getName(unsigned int);
		static unsigned int getMapID(unsigned int);
	};

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

	class GfMenuBdat {
	   public:
		static unsigned char* getItemDataFP(unsigned char itemType, unsigned short itemId);
		static unsigned char* getItemTextFP(unsigned char itemType, unsigned short itemId);
	};

	class GfMenuItemUtil {
	   public:
		static void getItemName(unsigned char itemType, unsigned int itemId, ui::UIStr& uistr);
		static void getItemTypeName(unsigned char itemType, ui::UIStr& uistr);
		static int getItemNum(unsigned char itemType, unsigned int itemId);
		static bool isModelChangeItem(unsigned int, unsigned int);
	};

	class GfDataItem {
	   public:
		static int getItemType(unsigned int itemId);
		static char* getCaption(unsigned int itemId);
	};

}