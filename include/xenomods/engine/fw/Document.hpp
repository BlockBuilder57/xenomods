//
// Created by block on 7/21/2022.
//

#pragma once

#include "UpdateInfo.hpp"
#include "xenomods/engine/ml/WinView.hpp"

#if XENOMODS_NEW_ENGINE
namespace fw {

	class Applet {
	   public:
		INSERT_PADDING_BYTES(0x1f8);
		ml::WinView* winView;
		INSERT_PADDING_BYTES(0x18);
		UpdateInfo updateInfo;

		void setupSystem();

		ml::Scn* getScn() const;
	};

	class Document /*: public game::DocAccessor*/ {
	   public:
		Applet* applet;
	};

} // namespace fw

namespace xenomods {

	extern fw::Document* DocumentPtr;

} // namespace xenomods
#endif