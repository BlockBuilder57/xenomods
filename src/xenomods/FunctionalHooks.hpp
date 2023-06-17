// Created by block on 6/17/23.

#pragma once

#include "skylaunch/hookng/Hooks.hpp"


namespace xenomods {

	// Cannot be anonymous, otherwise every use of it will be unique
	struct ClampNumberOfControllers : skylaunch::hook::Trampoline<ClampNumberOfControllers> {
		static int Hook();
	};

	class FunctionalHooks {
	   public:
		/*
		 * Performs all hooks that are functionality important
		 * (eg controllers, enabling debug rendering)
		 */
		static void Hook();
	};

} // namespace xenomods
