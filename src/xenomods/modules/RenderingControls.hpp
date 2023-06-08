#pragma once

#include "UpdatableModule.hpp"

namespace xenomods {

	struct RenderingControls : public xenomods::UpdatableModule {
		static bool skipUIRendering;
		static bool skipParticleRendering;
		static bool skipCloudRendering;
		static bool skipSkyDomeRendering;
		static bool straightenFont;

		struct RenderParmForces {
			bool DisableDOF;
			bool DisableMotionBlur;
			bool DisableColorFilter;

			bool Any() {
				return DisableDOF || DisableMotionBlur || DisableColorFilter;
			}
		};
		static RenderParmForces RenderParameters;

		void Initialize() override;
		void Update(fw::UpdateInfo* updateInfo) override;
		bool NeedsUpdate() const override {
			return true;
		}
	};

} // namespace xenomods
