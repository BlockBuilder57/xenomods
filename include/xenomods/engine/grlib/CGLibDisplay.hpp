// Created by block on 2026-03-23.

#pragma once

#include <xenomods/Utils.hpp>

#include "CGLibMemory.hpp"
#include "CGLibRenderTarget.hpp"

namespace grlib {

	class CGLibDisplay {
	   public:
		CGLibRenderTarget rtRender0;
		CGLibRenderTarget rtRender1;
		CGLibDepthTarget rtDepth;
		CGLibMemPool memPool;
		// more...

		void resize(uint width, uint height);
	};

} // namespace grlib