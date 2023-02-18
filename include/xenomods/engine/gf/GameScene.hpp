// Created by block on 2/11/2023.

#pragma once

#include <xenomods/Utils.hpp>

namespace gf {

	struct OwnerBonus {
		short unk1;
		short moveSpeedThingy;
		INSERT_PADDING_BYTES(28);
		float whoKnows;
	};

	class GfGameScene {
	   public:
		void* vtable;
		//INSERT_PADDING_BYTES(72);
		//OwnerBonus* ownerBonus;

		OwnerBonus* getOwnerBonus() const;
	};

}