// Created by block on 2026-03-23.

#pragma once

#include <xenomods/Utils.hpp>

namespace grlib {

	class CGLibTextureBuffer {
	   public:
		uint grlSurfaceFormat;
		ushort width;
		ushort height;
		ushort depth;
		ushort slice;
		ushort mipmap;
		ushort target;
		uint flags;
		INSERT_PADDING_BYTES(4);
		void* nvnTexture;
		void* bufferAddress;
		void* storageBuffer;
		uint storageSize;
	};

	// 0x70
	class CGLibRenderTargetBase {
	   public:
		CGLibTextureBuffer textureBuf;
		void* activeTexture;
	};

	// 0x3c8
	class CGLibRenderTarget : public CGLibRenderTargetBase {
	   public:
		INSERT_PADDING_BYTES(0x350);
	};

	class CGLibDepthTarget {
	   public:
		INSERT_PADDING_BYTES(0x800);
	};

} // namespace grlib