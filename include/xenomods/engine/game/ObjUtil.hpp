// Created by block on 3/21/23.

#pragma once

#include "xenomods/engine/fw/Document.hpp"

#include "CharacterController.hpp"

namespace game {

	class ObjUtil {
	   public:
		static void getPartyHandle(const fw::Document& doc, unsigned short index);
		static CharacterController* getCharacterController(const fw::Document& doc, unsigned int handle);
	};

}