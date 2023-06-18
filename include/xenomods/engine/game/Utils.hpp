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

#if XENOMODS_NEW_ENGINE
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
		static unsigned int searchBdatFldMaplistRowID(const fw::Document& doc, const char* id_name);
		static bool isDisableMenu(const fw::Document& doc);
		static bool isOpenableCurrentAreaMap(const fw::Document& doc);
		static void makeParty(const fw::Document& doc, const mm::mtl::FixedVector<unsigned short, 9ul>&);
		static DataParty* getDataParty(const fw::Document& doc);
		static DataPc* getDataPc(const fw::Document& doc, unsigned short pcid);
	};

	class ObjUtil {
	   public:
		static unsigned int getPartyHandle(const fw::Document& doc, unsigned short index);
		static CharacterController* getCharacterController(const fw::Document& doc, unsigned int handle);
	};

	namespace SeqUtil {
		struct WarpPcPartyParams {

		};

		void makeMapJumpSetupInfoFromLandmark(MapJumpSetupInfo& info, const fw::Document& doc, ushort landmarkId);
		void requestMapJump(const fw::Document& doc, const MapJumpSetupInfo& info);
		void returnTitle(const fw::Document& doc);
		void warpPcParty(const WarpPcPartyParams& params);
	}

}
#endif