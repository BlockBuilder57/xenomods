// Created by block on 3/21/23.

#pragma once

#include "CharacterController.hpp"
#include "xenomods/engine/fw/Document.hpp"

#if XENOMODS_CODENAME(bfsw)
namespace game {

	class ObjUtil {
	   public:
		static unsigned int getPartyHandle(const fw::Document& doc, unsigned short index);
		static CharacterController* getCharacterController(const fw::Document& doc, unsigned int handle);
	};

} // namespace game
#endif