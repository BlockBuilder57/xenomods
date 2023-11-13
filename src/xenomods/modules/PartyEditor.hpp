// Created by block on 6/18/23.

#pragma once

#include "UpdatableModule.hpp"
#include "xenomods/engine/game/Data.hpp"
#include "xenomods/engine/game/Utils.hpp"
#include "xenomods/engine/gf/Party.hpp"

namespace xenomods {

	struct PartyEditor : public UpdatableModule {
#if XENOMODS_OLD_ENGINE
		static gf::RQ_SETUP_PARTY PartySetup;
		static int PartyIdx;
		static int TeamIdx;

		static void MenuBaseParty();
		static void MenuIraParty();
#elif XENOMODS_CODENAME(bfsw)
		static game::PcID PartySetup[7];
		static game::PcID StatusCurrentPcID;
		static game::DataPc* StatusCurrentDataPc;
		static game::DataUtil::AddExpApSpInfo AddInfo;

		static void MenuOrder();
		static void MenuStatus();
		static void MenuAddExperience();
#endif

		static void MenuSection();

		void Initialize() override;
		bool NeedsUpdate() const override {
			return false;
		}
	};

} // namespace xenomods
