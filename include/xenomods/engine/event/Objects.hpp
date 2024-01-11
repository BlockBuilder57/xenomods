// Created by block on 1/11/24.

#pragma once

#include <xenomods/Utils.hpp>

#include "xenomods/engine/gf/EventModel.hpp"
#include "xenomods/engine/gf/Object.hpp"
#include "xenomods/engine/mm/mtl/FixStr.hpp"

#if XENOMODS_OLD_ENGINE

namespace event {

	enum class MODEL_TYPE {
		Driver = 1,
		Blade,
		Npc,
		Enemy,
		BladeWeapon
	};

	class CharaObj { // size 0x3060 (12384)
	   public:
		INSERT_PADDING_BYTES(0xCA8);
		mm::mtl::FixStr<64> objectName;
		INSERT_PADDING_BYTES(0x8);
		mm::mtl::FixStr<64> modelPath;
		unsigned int bdatIndex;
		unsigned int unk2;
		gf::GF_OBJ_HANDLE* objHandle;
		gf::GfEvtModel* evtModel;
		INSERT_PADDING_BYTES(8984);

		void initModel();
		void initModelStream();
	};

	class ModelObj {
	   public:
		struct OPTION {
			int unk1;
			int unk2;
		};

		gf::GfEvtModel* setupModel(MODEL_TYPE modelType, unsigned int id, const OPTION&);
	};

} // namespace event

#endif