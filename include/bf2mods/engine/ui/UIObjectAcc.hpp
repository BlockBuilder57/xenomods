//
// Created by block on 1/22/2023.
//

#pragma once

#include <bf2mods/Utils.hpp>

#include "UIStr.hpp"

namespace ui {

	class UIObjectAcc {
	   public:
		void* vtable;
		void* uiObject;
		unsigned int objectId;

		UIObjectAcc(uint id);
		UIObjectAcc(uint parent, const char* child);
		UIObjectAcc(const UIObjectAcc& other);

		char* getName();

		uint duplicate(uint id);
		uint duplicateChild(const char* name);

		void getPos(mm::Pnt<short>& ref) const;
		void setPos(const mm::Pnt<short>& ref);

		// I think mode 0 is center, mode 1 is local center, mode 2 is top-left
		void getRect(mm::Rect<short>& ref, unsigned short mode) const;

		void getText(ui::UIStr& ref) const;
		void setText(const ui::UIStr& ref);

		// doesn't function...?
		/*uchar getTextAlignH() const;
		uchar getTextAlignV() const;
		void setTextAlignH(uchar alignment);
		void setTextAlignV(uchar alignment);*/

		float getSlopeRot() const;
		void setSlopeRot(float value); // often set to -12?

		void setIgnoreSize(bool);

		void updateLayoutSize();

	};

}