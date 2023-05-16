//
// Created by block on 7/21/2022.
//

#pragma once

#include "xenomods/engine/fw/Document.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"

#if XENOMODS_NEW_ENGINE
namespace game {

	struct MapJumpSetupInfo {
		ushort chapter;
		ushort location;
		mm::Vec3 maybeVec;
		mm::Vec3 jump_pos;
		mm::Vec3 jump_rot;
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