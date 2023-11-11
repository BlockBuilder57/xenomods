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
			std::string mapName {}; // not serialized
			unsigned short mapId {};
			glm::vec3 position {};
			glm::quat rotation {};
			glm::vec3 rotationEuler {}; // not serialized
			glm::vec3 velocity {};
		};
		static std::vector<WarpData> Warps;

		static glm::vec3* GetPartyPosition();
		static void SetPartyPosition(glm::vec3 pos);
		static glm::quat* GetPartyRotation();
		static void SetPartyRotation(glm::quat rot);

		static glm::vec3* GetPartyVelocity();
		static void SetPartyVelocity(glm::vec3 vel);

		static WarpData* NewWarp();
		static void SetWarp(WarpData* warp);
		static void DeleteWarp(WarpData* warp);

		static void GoToWarp(WarpData* warp);

		static void LoadWarpsFromFile();
		static void SaveWarpsToFile();

		static void MenuSection();
		static void MenuState();

		static bool ShowWarpsWindow;
		static bool ShowAllWarps;
		static bool ShowWarpsOnMap;
		static void MenuWarps();

		void Initialize() override;
		bool NeedsUpdate() const override {
			return true;
		}
		void Update(fw::UpdateInfo* updateInfo) override;
		void OnMapChange(unsigned short mapId) override;
	};

} // namespace xenomods
