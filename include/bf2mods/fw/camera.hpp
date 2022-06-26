#pragma once

#include "../mm/math_types.hpp"
#include "../utils.hpp"

namespace fw {

	struct Camera {
		INSERT_PADDING_BYTES(64);
		mm::Vec3 position;
                int unk;
		mm::Vec3 lookAt;
	};

}