//
// Created by block on 12/27/2022.
//

#pragma once

#include "xenomods/Utils.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"

namespace ml {

	class CacheDraw {

	};

	class DebDraw {
	   public:
		INSERT_PADDING_BYTES(0x210);
		CacheDraw* pCacheDraw;
		bool notRender;

		static DebDraw* getOnly(uint32_t unused);
		static DebDraw* get(uint32_t maybeIndex); // use -1 for this
		//void setForceRenderQueue(bool enable);
		static unsigned int getCacheDrawWID();

	};

}
