// Created by block on 6/12/23.

#pragma once

#include <xenomods/Utils.hpp>

#include "xenomods/engine/fw/Document.hpp"

#if XENOMODS_CODENAME(bfsw)
namespace game {

	class MenuGameDataMap {
	   public:
		static unsigned short getPlayerStayingMapId(const fw::Document& doc);
	};

}
#endif