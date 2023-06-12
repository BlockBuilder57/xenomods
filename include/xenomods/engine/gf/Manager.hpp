//
// Created by block on 1/16/2023.
//

#pragma once

#include "Object.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"

namespace gf {

	struct GfGameManager {
	   public:
		static GF_OBJ_HANDLE* getControlMover();
		static mm::Vec3* getControlMoverPosition();
		static float* getControlMoverRotation(); // can't seem to use this without a crash

		static GF_OBJ_HANDLE* getPartyLeader();

		static bool isClearGame();
		static bool isControlFree();
	};

	struct GfMapManager {
	   public:
		static unsigned short getMapID();
		static unsigned short getMapJumpID();
		static char* getMapName();
		static char* getAssetName();
	};

	struct GfFieldManager {
	   public:
		static void clear(); // RESETS PLAYTIME!!
	};

} // namespace gf