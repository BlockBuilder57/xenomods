//
// Created by block on 7/21/2022.
//

#pragma once

#include "xenomods/Utils.hpp"

#include "xenomods/engine/fw/Document.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"

#if XENOMODS_CODENAME(bfsw)
namespace game {
	class ScriptCfs {
	   public:
		void returnTitle();
	};

	class ScriptUnit {
	   public:
		static void setWarp(void* handle, const mm::Vec3& pos);
		static void* getPartyHandle(unsigned short index);
	};

	class ScriptUtil {
	   public:
		static fw::Document* s_document;

		static fw::UpdateInfo* getUpdateInfo();
	};
} // namespace game
#endif