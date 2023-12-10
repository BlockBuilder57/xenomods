//
// Created by block on 6/28/2022.
//

#pragma once

#include "xenomods/Utils.hpp"
#include "xenomods/engine/ml/Scene.hpp"

namespace apps {

	class FrameworkLauncher {
	   public:
		INSERT_PADDING_BYTES(424)
		ml::IScnRender scnRender;
		INSERT_PADDING_BYTES(250); // travesty
		ml::Scn* ScnPtr; // 696
		ml::WinView* winView;
		INSERT_PADDING_BYTES(354);
		byte UpdateState; // 1050, controls updates

		void update();

		bool setDebugPause(bool);
		bool isDebugPause() const;
		bool stepDebugPause();
	};

} // namespace apps
