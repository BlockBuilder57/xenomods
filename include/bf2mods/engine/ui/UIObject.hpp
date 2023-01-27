//
// Created by block on 1/23/2023.
//

#pragma once

#include <bf2mods/engine/mm/FixStr.hpp>
#include <bf2mods/engine/layer/LayerBase.hpp>

#include "UIDisplayInfo.hpp"
#include "UIObjectPlugin.hpp"
#include "UITransform.hpp"

namespace ui {

	enum UIObjectBase_Flags : uint {
		kDuplicate = 0x10
	};

	class UIObjectBase {
	   public:
		void* vtable;
		void* unkptr;
		void* UIObjectBase1;
		void* UIObjectBase2;
		UITransform transform;
		int unk1;
		UIDisplayInfo displayInfo;
		UIObjectBase_Flags flags;
		int unk2;
		mm::mtl::FixStr<32> name;
	};

	class UIObject : public UIObjectBase {
	   public:
		int unk1;
		layer::LayerBase* layerBase;
		void* layerAccObjBase;
		short type;
		short pad;
		unsigned int parentId;
		UIObjectPlugin* latestPlugin;
		unsigned int objectId;
		unsigned int nameHash;

		static void importDisplaySize(UIDisplayInfo&, layer::LayerBase*);

		ui::UIObject* getParentPtr() const;

		void update();
	};

}