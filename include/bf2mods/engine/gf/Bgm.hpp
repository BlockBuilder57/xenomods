#pragma once

#include <map>

#include "bf2mods/engine/mm/FixStr.hpp"
#include "bf2mods/stuff/utils/util.hpp"

namespace gf {

	class BgmTrack {
	   public:
		struct VfTable {
			//std::intptr_t padding[2]; // all 0x00 filler
			void (*Dtor)();
			void (*DeletingDtor)();
			char* (*GetTrackName)(BgmTrack* this_pointer);
		};
		//void** vtable;
		INSERT_PADDING_BYTES(20);
		char playingBgmFileName[64];

		char* getPlayingBgmFileName() const;
		bool makePlayFileName(mm::mtl::FixStr<64>& fixStr) const;

		float getPlayTime() const;
		float getTotalTime() const;
		bool isLoop() const;
		bool isPlaying() const;
	};

	class BgmBattle : BgmTrack {
		bool makePlayFileName(mm::mtl::FixStr<64>* fixStr) const;
	};
	class BgmBattleBoss : BgmTrack {
		bool makePlayFileName(mm::mtl::FixStr<64>* fixStr) const;
	};
	class BgmEvent : BgmTrack {
		bool makePlayFileName(mm::mtl::FixStr<64>* fixStr) const;
	};
	class BgmField : BgmTrack {
		bool makePlayFileName(mm::mtl::FixStr<64>* fixStr) const;
	};
	class BgmGimmick : BgmTrack {
		bool makePlayFileName(mm::mtl::FixStr<64>* fixStr) const;
	};

} // namespace gf
