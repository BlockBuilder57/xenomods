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
		static void getItemHelp(const fw::Document& doc, unsigned short id, mm::mtl::FixStr<256>& str, unsigned char colorThing);
		static void getItemName(const fw::Document& doc, unsigned short id, mm::mtl::FixStr<256>& str);
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

}
#endif