// Created by block on 6/18/23.

#pragma once

#include <xenomods/Utils.hpp>

#include "CharacterController.hpp"
#include "xenomods/engine/fw/Document.hpp"

#if XENOMODS_CODENAME(bfsw)
namespace game {

	class DocAccessor {
		void* getDataManager();
		void* getPartyManager();
	}

}
#endif