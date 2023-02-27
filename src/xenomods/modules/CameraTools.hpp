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
#if XENOMODS_CODENAME(bfsw)
			mm::Mat44 matrixUI;
#endif
			float fov;
			float camSpeed;
		};
		static FreecamState Freecam;

		static void DoFreeCameraMovement(glm::mat4& matrix);

		void Initialize() override;
		bool NeedsUpdate() const override {
			return true;
		}
		void Update() override;
	};

} // namespace xenomods
