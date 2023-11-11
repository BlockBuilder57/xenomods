//
// Created by block on 6/21/2022.
//

#pragma once

#include "UpdatableModule.hpp"
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

		struct CameraMeta {
			glm::vec3 pos;
			glm::quat rot;
			glm::vec3 euler;
			glm::vec3 forward;
			glm::vec3 up;
			float fov;
		};
		static CameraMeta Meta;

		static void UpdateMeta();

		static void MenuSection();

		void Initialize() override;
		bool NeedsUpdate() const override {
			return true;
		}
		void Update(fw::UpdateInfo* updateInfo) override;
	};

} // namespace xenomods
