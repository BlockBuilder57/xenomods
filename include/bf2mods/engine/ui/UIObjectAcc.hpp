//
// Created by block on 1/22/2023.
//

#pragma once

#include <bf2mods/Utils.hpp>

#include "UIObject.hpp"
#include "UIStr.hpp"

namespace ui {

	class UIObjectAcc {
	   public:
		void* vtable;
		UIObject* uiObject;
		unsigned int objectId;

		UIObjectAcc(uint id);
		UIObjectAcc(uint parent, const char* child);
		UIObjectAcc(const UIObjectAcc& other);

		char* getName();

		uint duplicate(uint id);
		uint duplicateChild(const char* name);

		void getPos(mm::Pnt<short>& ref) const;
		void setPos(const mm::Pnt<short>& ref);

		/**
		 * Weirdly complicated function for getting the rect of a UI object
		 * The mode param seems to be a bitflag? Certain bits are tested for different behaviors
		 * Normally, the rect is set based on the UITransform position and UIDisplayInfo size
		 *
		 * Bit 0 seems to only control if the parent objects are looped through and factored in to rect size
		 * Bit 1 seems to determine two things:
		 * - if the display object is added when getting pos (from a display(/centering?) mode + font pixel x/y)
		 * - if scale is accounted for when getting size
		 *
		 * A nominal mode would probably just be 2
		 */
		void getRect(mm::Rect<short>& ref, unsigned short mode) const;

		void getText(ui::UIStr& ref) const;
		void setText(const ui::UIStr& ref);

		uchar getTextAlignH() const;
		uchar getTextAlignV() const;
		void setTextAlignH(uchar alignment);
		void setTextAlignV(uchar alignment);

		float getSlopeRot() const;
		void setSlopeRot(float value); // often set to -12?

		void setIgnoreSize(bool);

		void updateLayoutSize();

	};

}