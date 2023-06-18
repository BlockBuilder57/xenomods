// Created by block on 6/18/23.

#pragma once

#include "../UpdatableModule.hpp"
#include "xenomods/engine/game/DataPC.hpp"

#if XENOMODS_CODENAME(bfsw) && _DEBUG
namespace xenomods {

	struct PartyEditor : public UpdatableModule {
		static game::PcID CurPcID;

		void Initialize() override;
		bool NeedsUpdate() const override {
			return false;
		}
	};

} // namespace xenomods
#endif
