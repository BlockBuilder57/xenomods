//
// Created by block on 1/26/2023.
//

#pragma once

#include <bf2mods/Utils.hpp>

namespace ui {

	// temporary, move somewhere else
	struct RTTIObj {
		const char* szName;

		RTTIObj* pPrevInheritanceObj;
		void* unk;
	};

	class UIObjectPlugin {
	   public:
		int m_RefCount; // from mm::mtl::ReferenceObject;
		INSERT_PADDING_BYTES(0x4);
		UIObjectPlugin* prevPlugin;

		virtual ~UIObjectPlugin() = default;

		virtual RTTIObj* getRTTI() const;
		virtual void reload();
		virtual void execute();

		virtual bool isNeedUpdate();

	};

}