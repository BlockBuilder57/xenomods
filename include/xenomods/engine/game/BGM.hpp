// Created by block on 8/26/23.

#pragma once

#include <xenomods/Utils.hpp>

#include "Controllers.hpp"

#if XENOMODS_CODENAME(bfsw)
namespace game {

	class BGM {
	   public:
		std::uint16_t getBattleBGMID(const GameController& controller) const;
		bool testTensionZero(const GameController& controller) const;
	};

}
#endif