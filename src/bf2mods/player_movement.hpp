#pragma once

#include "bf2mods/UpdatableModule.hpp"

#include <bf2mods/engine/mm/math_types.hpp>

namespace bf2mods {

	struct PlayerMovement : public bf2mods::UpdatableModule {
		void Initialize() override;
	};

} // namespace bf2mods
