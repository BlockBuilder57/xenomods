#pragma once

#include "modules/UpdatableModule.hpp"

#include <xenomods/State.hpp>
#include <nn/hid.hpp>

#include "xenomods/engine/mm/MathTypes.hpp"

namespace xenomods {

	/**
 	 * Called on each Framework update.
 	 */
	void update(fw::UpdateInfo* updateInfo);

	/**
 	 * Called from skylaunch when ready.
 	 */
	void main();

} // namespace xenomods
