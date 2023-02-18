//
// Created by block on 1/23/2023.
//

#pragma once

#include <xenomods/engine/mm/MathTypes.hpp>

namespace ui {

	struct UITransform {
	   public:
		void* vtable;
		mm::Pnt<short> pos;
		int unk;
		mm::Vec2 scale;
		void* unk2;
		float rotation;
		bool isDirty;
	};

}