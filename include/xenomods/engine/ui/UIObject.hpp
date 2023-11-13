//
// Created by block on 1/23/2023.
//

#pragma once

#include <xenomods/engine/layer/LayerBase.hpp>

#include "UIDisplayInfo.hpp"
#include "UIObjectPlugin.hpp"
#include "UIStr.hpp"
#include "UITransform.hpp"
#include "xenomods/engine/mm/mtl/FixStr.hpp"
#include "xenomods/engine/mm/mtl/RTTI.hpp"

namespace ui {



	class UIObjectBase {
	   public:

		enum class Flags : uint {
			kDuplicate = 0x10
		};

		void* unkptr;
		void* UIObjectBase1;
		void* UIObjectBase2;
		UITransform transform;
		int unk1;
		UIDisplayInfo displayInfo;
		Flags flags;
		int unk2;
		mm::mtl::FixStr<32> name;

		virtual ~UIObjectBase();
		virtual void update();
		virtual void setInitialize();
		virtual void setTerm();
	};

	class UIObject : public UIObjectBase {
	   public:

		enum class Type : short {
			kInvalid = 0x0,
			kText = 0x1,
			kLayout = 0x3,
			kPolygon = 0x5,
			kImagesAboveThis = 0x7
		};

		int unk1;
		layer::LayerBase* layerBase;
		void* layerAccObjBase;
		Type type;
		short pad;
		unsigned int parentId;
		UIObjectPlugin* latestPlugin;
		unsigned int objectId;
		unsigned int nameHash;

		static void importDisplaySize(UIDisplayInfo&, layer::LayerBase*);

		ui::UIObject* getParentPtr() const;

		void update();

		virtual mm::mtl::RTTI* getRTTI() const;
		virtual void apply();
		virtual void getDisplayOffset(unsigned short, mm::Pnt<short>&) const;
		virtual void getTransformPosOffs(mm::Vec2&);
		virtual unsigned int duplicateImpl(unsigned int);
	};

	class UITextObject : UIObject {
	   public:
		static mm::mtl::RTTI m_rtti;
	};

}