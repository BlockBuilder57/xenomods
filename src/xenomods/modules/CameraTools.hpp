//
// Created by block on 6/21/2022.
//

#pragma once

#include"UpdatableModule.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"

namespace xenomods {

	struct CameraTools : public xenomods::UpdatableModule {
		struct FreecamState {
			bool isOn;
			mm::Mat44 matrix;
			float fov;
			float camSpeed;
		};
		static FreecamState Freecam;

		struct FreecamMeta {
			glm::vec3 pos;
			glm::vec3 forward;
			glm::vec3 up;
			glm::quat rot;
		};
		static FreecamMeta Meta;

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
		bool NeedsUpdate() const override {
			return true;
		}
		void Update(fw::UpdateInfo* updateInfo) override;
	};

} // namespace xenomods
