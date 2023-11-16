//
// Created by block on 6/21/2022.
//

#pragma once

#include "UpdatableModule.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"

namespace xenomods {

	struct CameraTools : public xenomods::UpdatableModule {
		struct FreecamSettings {
			enum class MoveAxis {
				XZ,
				XY,
				YZ
			};

			bool freecamOn;
			bool relativeToPlayer;
			MoveAxis moveAxis;
			bool isFreezePos[3];
			bool isGlobalPos[3];
			bool isGlobalRot[3];
			float camSpeed;

			bool enableTargeting;
			bool targetFollowPlayer;
			glm::vec3 targetPos;
		};
		static FreecamSettings Settings;

		struct CameraState {
			mm::Mat44 matrix;
			float fov;
		};
		static CameraState CamState;

		struct CameraMeta {
			glm::vec3 pos;
			glm::quat rot;
			glm::vec3 euler;
			glm::vec3 forward;
			glm::vec3 up;
			float fov;
		};
		static CameraMeta CamMeta;

		static void UpdateMeta();

		static void MenuSection();
		static void MenuSettings();

		void Initialize() override;
		bool NeedsUpdate() const override {
			return true;
		}
		void Update(fw::UpdateInfo* updateInfo) override;
	};

} // namespace xenomods
