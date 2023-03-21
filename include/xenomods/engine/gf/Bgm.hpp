#pragma once

#include <map>

#include "xenomods/engine/mm/FixStr.hpp"
#include "xenomods/stuff/utils/util.hpp"

namespace gf {

	class BgmTrack {
	   public:
		INSERT_PADDING_BYTES(20);
		char playingBgmFileName[64];

		virtual ~BgmTrack() = default;

		virtual char* getTrackName() const;
		virtual bool isPlaying() const;
		virtual void update(const fw::UpdateInfo& update);

		char* getPlayingBgmFileName() const;
		bool makePlayFileName(mm::mtl::FixStr<64>& fixStr) const;

		float getPlayTime() const;
		float getTotalTime() const;
		bool isLoop() const;
	};

} // namespace gf
