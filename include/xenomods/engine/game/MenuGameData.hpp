// Created by block on 6/12/23.

#pragma once

#include <xenomods/Utils.hpp>

#include "xenomods/engine/fw/Document.hpp"

#include "Bdat.hpp"

#if XENOMODS_CODENAME(bfsw)
namespace game {

	class MenuGameDataMap {
	   public:
		enum class MapType {
			OneFloor,
			MultipleFloors, // this one is the most used
		};

		MenuGameDataMap(const fw::Document& doc);
		bool create(int id, MapType type);

		bool isValid() const;

		bool isMapLandmarkAllVisibled() const;
		bool isLandmarkVisibleCondition(int id) const;
		bool isLandmarkLocked(int id) const;
		bool isLandmarkLockedByLandmarkLockFlag() const;

		unsigned short getPlayerStayingMapId() const;
		static unsigned short getPlayerStayingMapId(const fw::Document& doc);

		MsText getMapNameText();
		MsText getLandmarkNameText(int id);

		bool isValidFloorId(int floor) const;
		bool isPlayerStayingInCurrentMap() const;
	};

}
#endif