// Created by block on 1/11/24.

#pragma once

#include <xenomods/Utils.hpp>

#include "Object.hpp"

#if XENOMODS_OLD_ENGINE

namespace gf {

	class GfEvtModel {
	   public:
		INSERT_PADDING_BYTES(0x20);
		gf::GF_OBJ_HANDLE* objHandle;
	};

	class GfEvtModelBlade : public GfEvtModel {};
	class GfEvtModelDriver : public GfEvtModel {};
	class GfEvtModelEnemy : public GfEvtModel {};
	class GfEvtModelEvent : public GfEvtModel {};
	class GfEvtModelNpc : public GfEvtModel {};
	class GfEvtModelSalvage : public GfEvtModel {};
	class GfEvtModelWeapon : public GfEvtModel {};

} // namespace gf

#endif