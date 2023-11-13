// Created by block on 6/12/23.

#pragma once

#include <xenomods/Utils.hpp>

#include "xenomods/engine/fw/Document.hpp"

#include "Bdat.hpp"

#if XENOMODS_CODENAME(bfsw)
namespace game {

	class MenuGameDataPc {
	   public:
		MenuGameDataPc(const fw::Document& doc);
		void create(unsigned int pcid);
		bool refresh();
		void createFromPcId(PcID pcid);
		void createFromDataPc(DataPc* dataPc);

		const char* getName() const;
		MsText getNameMsText() const;

		int getLevel() const;
		int getHp() const;
		int getMaxHp() const;
		int getSp() const;
		int getNextSp() const;
		int getExp() const;
		int getNextExp() const;
		int getShortageExp() const;
		int getNextExpRate() const;

		const char* getActiveSkillLineName();
	};

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