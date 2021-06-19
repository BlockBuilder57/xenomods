#pragma once

#include <bf2mods/mm/math_types.hpp>

namespace fw {
	namespace debug {
		extern bool (*drawAxis)(const mm::Mat44* transform, float scale);
		extern bool (*drawFont)(int x, int y, const mm::Col4* color, const char* fmt, ...);
		extern int (*drawFontGetWidth)(const char* fmt, ...);
		extern int (*drawFontGetHeight)(const char* fmt, ...);
		extern void (*drawCompareZ)(bool compare);
	}
}

namespace gf {
	namespace GfMenuObjUtil {
		/*
		 * Anything (lower|camel)case has an unknown real name
		 */
		enum SEIndex: unsigned int {
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
	}
}

namespace bf2mods {

	extern int bgmTrackIndex;

	void DoMapJump(unsigned int mapjumpId);
	void PlaySE(unsigned int soundEffect);
	void PlaySE(gf::GfMenuObjUtil::SEIndex soundEffect);
	void ReturnTitle(unsigned int slot);

	template<typename... Args> bool DrawDebugFont(int x, int y, const char* fmt, Args... args);

	void SetupDebugStuff();

} // namespace bf2mods
