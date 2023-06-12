//
// Created by block on 7/21/2022.
//

#pragma once

#include "xenomods/Utils.hpp"
#include "xenomods/engine/fw/Document.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"

#if XENOMODS_NEW_ENGINE
namespace game {
	class ScriptCfs {
	   public:
		void returnTitle();
		static unsigned short getMapId();
	};

	class ScriptUnit {
	   public:
		static void setWarp(void* handle, const mm::Vec3& pos);
		static void* getPartyHandle(unsigned short index);
	};
} // namespace game
#endif