#pragma once

#include "UpdatableModule.hpp"
#include "xenomods/engine/ml/Scene.hpp"

namespace xenomods {

	struct RenderingControls : public xenomods::UpdatableModule {
		static bool straightenFont;
		static bool skipUIRendering;
		static bool skipParticleRendering;
		static bool skipOverlayRendering;
		static bool skipCloudRendering;
		static bool skipSkyDomeRendering;

		static float shadowStrength;

		static bool freezeTextureStreaming;

		struct ForcedRenderParameters {
			bool DisableDOF;
			bool DisableMotionBlur;
			bool DisableColorFilter;

			bool Any() {
				return DisableDOF || DisableMotionBlur || DisableColorFilter;
			}
		};
		static ForcedRenderParameters ForcedParameters;

		static void MenuSection();
		static void MenuToggles();
		static void MenuGBuffer();

		void Initialize() override;
		void Update(fw::UpdateInfo* updateInfo) override;
		bool NeedsUpdate() const override {
			return true;
		}
	};

} // namespace xenomods
