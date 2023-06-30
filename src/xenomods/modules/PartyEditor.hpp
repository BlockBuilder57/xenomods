// Created by block on 6/18/23.

#pragma once

#include "UpdatableModule.hpp"
#include "xenomods/engine/game/DataPC.hpp"
#include "xenomods/engine/gf/Party.hpp"

namespace xenomods {

	struct PartyEditor : public UpdatableModule {
#if XENOMODS_OLD_ENGINE
		static gf::RQ_SETUP_PARTY PartySetup;
		static int DriverIdx;
#elif XENOMODS_CODENAME(bfsw)
		static game::PcID PartySetup[7];
#endif

		void Initialize() override;
		bool NeedsUpdate() const override {
			return false;
		}
	};

} // namespace xenomods
