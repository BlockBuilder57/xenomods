//
// Created by block on 1/26/2023.
//

#pragma once

#include <bf2mods/Utils.hpp>

#include "bf2mods/engine/mm/mtl/RTTI.hpp"

namespace ui {


	class UIObjectPlugin {
	   public:
		int m_RefCount; // from mm::mtl::ReferenceObject;
		INSERT_PADDING_BYTES(0x4);
		UIObjectPlugin* prevPlugin;

		virtual ~UIObjectPlugin() = default;

		virtual mm::mtl::RTTI* getRTTI() const;
		virtual void reload();
		virtual void execute();

		virtual bool isNeedUpdate();

	};

}