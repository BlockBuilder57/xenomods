//
// Created by block on 7/21/2022.
//

// this seems to be DE exclusive

#pragma once

#include "UpdateInfo.hpp"

#if XENOMODS_NEW_ENGINE
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

} // namespace fw

namespace xenomods {

	extern fw::Document* DocumentPtr;

} // namespace xenomods
#endif