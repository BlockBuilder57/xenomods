//
// Created by block on 1/16/2023.
//

#pragma once

#include "IGfEvent.hpp"
#include "Object.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"

namespace gf {

	struct GfManager {
	   public:
		static bool isGameTypeIra();
	};

	struct GfGameManager {
	   public:
		static GF_OBJ_HANDLE* getControlMover();
		static mm::Vec3* getControlMoverPosition();
		static float* getControlMoverRotation(); // can't seem to use this without a crash

		static GF_OBJ_HANDLE* getPartyLeader();

		static bool isClearGame();
		static bool isControlFree();

		static bool isGameTypeIra();
	};

	struct GfMapManager {
	   public:
		static unsigned short getMapID();
		static unsigned short getMapJumpID();
		static char* getMapName();
		static char* getAssetName();

		static void gevMapjump(GEV gev, const MapjumpCallbackInfo& mapjumpInfo);
	};

	struct GfFieldManager {
	   public:
		static void clear(); // RESETS PLAYTIME!!
	};

} // namespace gf