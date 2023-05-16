// Created by block on 5/16/2023.

#pragma once

#include "xenomods/Utils.hpp"

#include "xenomods/engine/ml/DriverInterfaces.hpp"
#include "xenomods/engine/ml/Scene.hpp"
#include "xenomods/engine/mtl/Allocator.hpp"

namespace layer {

	class LayerManager {
	   public:
		INSERT_PADDING_BYTES(0x1C);
		void* updateRateRedHerring;

#if XENOMODS_OLD_ENGINE
		INSERT_PADDING_BYTES(0x4C);
#elif XENOMODS_CODENAME(bfsw)
		INSERT_PADDING_BYTES(0x3C);
#elif XENOMODS_CODENAME(bf3)
		INSERT_PADDING_BYTES(0x48);
#endif
		float frameRateFreq;
		float frameRateTime;
		float anotherThing;

		LayerManager(ml::Scn*, mtl::ALLOC_HANDLE, unsigned int);

		virtual void finalRender(ml::IDrDrawWorkInfo*);
	};

}