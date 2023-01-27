//
// Created by block on 1/24/2023.
//

#pragma once

#include <bf2mods/engine/mm/MathTypes.hpp>

namespace ui {

	class UIDisplayInfo {
	   public:
		void* vtable;
		bool isDisp;
		INSERT_PADDING_BYTES(0x3);
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