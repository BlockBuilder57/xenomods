#pragma once

#include "UpdatableModule.hpp"
#include "bf2mods/engine/mm/MathTypes.hpp"

namespace xenomods {

	struct PlayerMovement : public xenomods::UpdatableModule {
		static bool moonJump;
		static bool disableFallDamage;
		static float movementSpeedMult;

		void Initialize() override;
		bool NeedsUpdate() const override {
			return true;
		}
		void Update() override;
	};

} // namespace xenomods
