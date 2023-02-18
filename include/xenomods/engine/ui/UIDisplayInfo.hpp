//
// Created by block on 1/24/2023.
//

#pragma once

#include <xenomods/engine/mm/MathTypes.hpp>

namespace ui {

	class UIDisplayInfo {
	   public:
		enum class DisplayMode : byte {
			kZero = 0,
			kOne
		};

		void* vtable;
		bool isDisp;
		DisplayMode displayMode;
		INSERT_PADDING_BYTES(0x2);
		short priority;
		short unk2;
		mm::Col4 multiplyColor;
		INSERT_PADDING_BYTES(0x8+0x4);
		mm::Pnt<short> size;
		bool isDirty;
		INSERT_PADDING_BYTES(0x3);
		INSERT_PADDING_BYTES(0x4+0x8+0x8);
	};

}