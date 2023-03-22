//
// Created by block on 7/21/2022.
//

// this seems to be DE exclusive

#pragma once

#include "UpdateInfo.hpp"

namespace fw {

	class Applet {
	   public:
		INSERT_PADDING_BYTES(0x218);
		UpdateInfo updateInfo;
	};

	class Document {
	   public:
		Applet* applet;
	};

	extern Document* document;

} // namespace fw