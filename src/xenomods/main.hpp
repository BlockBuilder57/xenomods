#pragma once

#include <xenomods/engine/mm/MathTypes.hpp>
#include <nn/hid.hpp>

#include "State.hpp"
#include "modules/UpdatableModule.hpp"

namespace xenomods {

	/**
 	 * Called on each Framework update.
 	 */
	void update();

	/**
 	 * Called from skylaunch when ready.
 	 */
	void main();

} // namespace xenomods
