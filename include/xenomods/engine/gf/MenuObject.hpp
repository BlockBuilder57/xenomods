//
// Created by block on 7/31/2022.
//

#pragma once

#include <xenomods/Utils.hpp>

#include "xenomods/engine/mm/MathTypes.hpp"

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

		void playSE(unsigned int index);
	} // namespace GfMenuObjUtil

	class GfMenuObjTitle {
	   public:
		INSERT_PADDING_BYTES(0x18);
		uint rootUIObjAcc;

		void initialize();
	};

	struct MenuZoneMapInfo {};

	class GfMenuObjWorldMap {
	   public:
		static bool chkMapCond(unsigned int mapjump, mm::Pnt<short>* pos);

		static bool isDispMap(unsigned int mapjump);
		static bool isEnableJump(const MenuZoneMapInfo& info);
		static bool isEnterMap(unsigned int mapjump);

		// DOES NOT EXIST IN 1.0.0
		static bool isOpenLandmark(unsigned int mapjump);
	};

	class GfMenuObjZoneMap {
	   public:
		static bool isEnableJump(const MenuZoneMapInfo& info);
	};

} // namespace gf