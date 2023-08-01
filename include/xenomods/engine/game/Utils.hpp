//
// Created by block on 7/21/2022.
//

#pragma once

#include <xenomods/Utils.hpp>

#include "CharacterController.hpp"
#include "DataPC.hpp"
#include "xenomods/engine/fw/Document.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"
#include "xenomods/engine/mm/mtl/FixedVector.hpp"

#if XENOMODS_CODENAME(bfsw)
namespace game {

	struct MapJumpSetupInfo {
		ushort chapter;
		ushort location;
		mm::Vec3 maybeVec;
		mm::Vec3 jump_pos;
		mm::Vec3 jump_rot;
		INSERT_PADDING_BYTES(0x20);
	};

	class DataItem {
	   public:
		struct DataCommon {
			ushort unk1;
			ushort unk2;
			ushort itemId;
			ushort itemType;
			ushort stackCount;
			int addTime;
			bool isInInventory;
			bool isFavorite;
		};

		// Gems and mob crystals
		struct DataCrystal : public DataCommon {
			ushort slotSize;
			byte rankType;
			byte atrType;
			byte gemIds;
			INSERT_PADDING_BYTES(1);
			byte skillCount;
			INSERT_PADDING_BYTES(1);
			ushort unk3;
			byte arrSkills;
			byte skillRate;
			INSERT_PADDING_BYTES(4);
			byte alsoCountUnk;
			INSERT_PADDING_BYTES(3);
		};

		struct DataSlot {
			uint itemId;
			ushort skillId;
			bool isEquipped;
		};

		// Weapons and armor
		struct DataEquip : public DataCommon {
			INSERT_PADDING_BYTES(1);
			byte slotCount;
			INSERT_PADDING_BYTES(2);
			DataSlot slots[3];
		};

		DataEquip Weapons[500];
		DataEquip HeadArmor[500];
		DataEquip BodyArmor[500];
		DataEquip ArmArmor[500];
		DataEquip LeggArmor[500];
		DataEquip FootArmor[500];
		DataCrystal Crystals[550];
		DataCrystal Gems[550];
		DataCommon Collectables[500];
		DataCommon Materials[500];
		DataCommon KeyItems[500];
		DataCommon ArtBooks[500];
		INSERT_PADDING_BYTES(263279-228000);
		DataEquip TempWeapons[32];
		DataEquip TempHeadArmor[32];
		DataEquip TempBodyArmor[32];
		DataEquip TempArmArmor[32];
		DataEquip TempLeggArmor[32];
		DataEquip TempFootArmor[32];
		DataCrystal TempGems[35];

		DataCommon* getItem(unsigned int);
	};

	class DataUtil {
	   public:
		struct AddExpApSpInfo {
			PcID pcid;
			int exp;
			int ap;
			int sp;
			bool unk1;
			bool showOnUI;
			bool applyBonusExp; // ?
		};

		static void addExpApSp(const fw::Document& doc, const AddExpApSpInfo& addInfo, bool clampThing);
		static int addItem(const fw::Document& doc, unsigned short id, unsigned short count, bool param_4, unsigned char showNotif, bool isTemp);
		static DataParty* getDataParty(const fw::Document& doc);
		static DataPc* getDataPc(const fw::Document& doc, unsigned short pcid);
		static DataItem* getDataItem(const fw::Document& doc);
		static void getItemHelp(const fw::Document& doc, unsigned short id, mm::mtl::FixStr<256>& str, unsigned char colorThing);
		static void getItemName(const fw::Document& doc, unsigned short id, mm::mtl::FixStr<256>& str);
		static DataItem::DataCommon* findStackItem(const fw::Document& doc, unsigned short id);
		static bool isDisableMenu(const fw::Document& doc);
		static bool isOpenableCurrentAreaMap(const fw::Document& doc);
		static void makeParty(const fw::Document& doc, const mm::mtl::FixedVector<unsigned short, 9ul>&);
		static unsigned int searchBdatFldMaplistRowID(const fw::Document& doc, const char* id_name);
	};

	class ObjUtil {
	   public:
		static unsigned int getPartyHandle(const fw::Document& doc, unsigned short index);
		static CharacterController* getCharacterController(const fw::Document& doc, unsigned int handle);
	};

	class ScriptUtil {
	   public:
		static fw::Document* s_document;
	};

	class SeqUtil {
	   public:
		struct WarpPcPartyParams {

		};

		static void makeMapJumpSetupInfoFromLandmark(MapJumpSetupInfo& info, const fw::Document& doc, ushort landmarkId);
		static void requestMapJump(const fw::Document& doc, const MapJumpSetupInfo& info);
		static void returnTitle(const fw::Document& doc);
		static void warpPcParty(const WarpPcPartyParams& params);
	};

	class CameraUtil {
	   public:
		static bool calcScreenPos(mm::Vec3& point, const fw::Document& doc, const mm::Vec3& pos);
	};

}
#endif