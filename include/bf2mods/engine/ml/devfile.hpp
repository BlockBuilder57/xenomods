//
// Created by block on 1/8/2023.
//

#pragma once

#include <bf2mods/utils.hpp>

#include "bf2mods/engine/mm/fixstr.hpp"

namespace ml {

	struct DevFileParamTh { /* PlaceHolder Structure */
		mm::mtl::FixStr<256>* RequestedFilename;
		int field1_0x8;
		int field2_0xc;
		void* field3_0x10;
		INSERT_PADDING_BYTES(31);
		long field36_0x38;
		void* readBuffer;
		uint32_t readSize;
		uint32_t readStartOffset;
		uint32_t field40_0x50;
		uint32_t field41_0x54;
		uint32_t field42_0x58;
		uint32_t field43_0x5c;
		void* userData;
		int field45_0x68;
		uint8_t field46_0x6c;
		uint8_t field47_0x6d;
		uint16_t field48_0x6e;
	};

} // namespace ml