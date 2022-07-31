//
// Created by block on 7/21/2022.
//

#pragma once

#include "bf2mods/engine/fw/document.hpp"
#include "bf2mods/engine/mm/math_types.hpp"

namespace game {

	struct MapJumpSetupInfo {
		ushort chapter;
		ushort location;
		mm::Vec3 maybeVec;
		mm::Vec3 jump_pos;
		mm::Vec3 jump_rot;
	};

	namespace SeqUtil {
		void makeMapJumpSetupInfoFromLandmark(MapJumpSetupInfo& info, const fw::Document& doc, ushort landmarkId);
		void requestMapJump(const fw::Document& doc, const MapJumpSetupInfo& info);
		void returnTitle(const fw::Document& doc);
	}

}