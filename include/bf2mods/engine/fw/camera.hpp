#pragma once

#include "bf2mods/engine/mm/math_types.hpp"
#include "bf2mods/utils.hpp"

namespace fw {

	struct Camera {
		INSERT_PADDING_BYTES(64);
		mm::Vec3 position;
		int unk;
		mm::Vec3 lookAt;
	};

} // namespace fw