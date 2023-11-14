//
// Created by block on 1/27/2023.
//

#pragma once

#include <xenomods/engine/mm/MathTypes.hpp>

#if XENOMODS_OLD_ENGINE
namespace gf {

	struct GfItemUtil {
		static bool addItemBox(unsigned int itemId, int count);
	};

	namespace util {
		bool getScreenPos(mm::Vec3& point, const mm::Vec3& pos);
	}

} // namespace gf
#endif
