//
// Created by block on 7/31/2022.
//

#pragma once

namespace gf {
	namespace GfMenuObjUtil {
		/*
		 * Anything (lower|camel)case has an unknown real name
		 */
		enum SEIndex : unsigned int {
			Decide = 1,
			Cancel = 2,
			menuopen = 3,
			menuclose = 4,
			Tab = 5,
			Cursor = 6,
			error = 7,
			setsomething = 8,
			opendialog = 9,
			sidedialog = 10,
			Sort = 11,
			OpenSubMenu = 12,
			tabSubMenu = 13, // switching driver or blade stat display
			CloseSubMenu = 14,
			affinityUnlock = 15,
			affinityUnlock2 = 16,
			unknown1 = 17,
			applyAuxCore = 18,
			poppiswapApply = 19,
			poppiswapCraft = 20,

			mapjump = 23,

			purchase = 24,
			notification = 29,

			textBubbleOpen = 55,
			textBubbleClose = 56,
			textBubbleThought = 57
		};

		void playSE(SEIndex index);
	} // namespace GfMenuObjUtil
} // namespace gf