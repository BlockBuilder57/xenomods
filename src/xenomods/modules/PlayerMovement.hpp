#pragma once

#include "UpdatableModule.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"

namespace xenomods {

	struct PlayerMovement : public xenomods::UpdatableModule {
		static bool moonJump;
		static bool disableFallDamage;
		static float movementSpeedMult;

		struct WarpData {
			std::string name {};
			unsigned short mapId {};
			std::string mapName {}; // not serialized
			glm::vec3 position {};
			glm::quat rotation {};
			glm::vec3 velocity {};
		};
		static std::vector<WarpData> Warps;
		static int WarpIndex;

		static glm::vec3 GetPartyPosition();
		static void SetPartyPosition(glm::vec3 pos);
		static glm::quat GetPartyRotation();
		static void SetPartyRotation(glm::quat rot);

		static glm::vec3 GetPartyVelocity();
		static void SetPartyVelocity(glm::vec3 vel);

		static void NewWarp();
		static void OverwriteWarp();
		static void GoToWarp();

		static void LoadWarpsFromFile();
		static void SaveWarpsToFile();

		void Initialize() override;

		bool NeedsUpdate() const override {
			return true;
		}
		void Update(fw::UpdateInfo* updateInfo) override;
	};

} // namespace xenomods
