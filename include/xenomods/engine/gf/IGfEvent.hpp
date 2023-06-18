// Created by block on 6/17/23.

#pragma once

#include <xenomods/Utils.hpp>

namespace gf {

	enum class GEV {
		NewMap = 4,
		EventCompleted = 10
	};

	struct MapjumpCallbackInfo {
	   public:
		unsigned int mapjumpId;
		int mapId;
		int unk1;
		int unk2;
		int unk3;
	};

	class IGfEvent {
		const void gevMapjump(GEV gev, const MapjumpCallbackInfo& mapjumpInfo);
	};

}