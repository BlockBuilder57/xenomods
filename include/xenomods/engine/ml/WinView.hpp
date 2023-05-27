// Created by block on 2/12/2023.

#pragma once

#include <xenomods/Utils.hpp>

#include "DebugDrawing.hpp"
#include "Scene.hpp"
#include "xenomods/engine/mm/FixStr.hpp"

namespace ml {

	struct WinView {
		INSERT_PADDING_BYTES(0x8);
		mm::mtl::FixStr<64> windowName;
		int idk;
		unsigned int windowID; // 80
		INSERT_PADDING_BYTES(0x150);
		void* fontLayer;
		INSERT_PADDING_BYTES(0x200);
		unsigned int renderingFlags;
	};

} // namespace ml