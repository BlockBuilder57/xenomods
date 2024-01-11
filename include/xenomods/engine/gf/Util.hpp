//
// Created by block on 1/27/2023.
//

#pragma once

#include <xenomods/engine/mm/MathTypes.hpp>

#include "Object.hpp"

#if XENOMODS_OLD_ENGINE
namespace gf {

	struct GfGameUtil {
		// base form means not using the condition model
		static char* getBladeModel(unsigned int bladeId, bool baseForm = false);
		static char* getDriverModel(unsigned int driverId, bool unk1, bool unk2);

		static void dispHomuraCrystal(gf::GF_OBJ_HANDLE* handle, bool on);
	};

	struct GfItemUtil {
		static bool addItemBox(unsigned int itemId, int count);
	};

	namespace util {
		bool getScreenPos(mm::Vec3& point, const mm::Vec3& pos);
	}

} // namespace gf
#endif
