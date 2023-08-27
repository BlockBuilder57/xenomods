// Created by block on 8/25/23.

#pragma once

#include <xenomods/Utils.hpp>

#if XENOMODS_CODENAME(bfsw)
namespace xefb {

	struct CSchedule {
	};

	class CEffect {
	};

	struct CERes {
	};

	enum class eXefbDrayType {
		None = -1,

		PostLine = 4,
		PostGauss = 5,
		PostRadial = 6,
		PostMono = 7,
		EnvTrail = 8,
		PostDist = 12,

		// unknown position
		EnvChain,
		EnvSprite
	};

	struct CEEmitter {
	   public:
		eXefbDrayType drawType;
	};

	class CEParticlelist {
	   public:
		void* vtable;
		int unk1;
		short unk2;
		short unk3;
		CSchedule* Schedule;
		CEEmitter* Emitter;

		void draw(CERes* res);
	};

} // namespace xefb
#endif