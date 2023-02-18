// Created by block on 2/12/2023.

#include <xenomods/Utils.hpp>

namespace ml {

	struct WinView {
		INSERT_PADDING_BYTES(0x50);
		unsigned int windowID;
	};

} // namespace ml