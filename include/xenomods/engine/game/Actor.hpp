// Created by block on 2/21/2023.

#pragma once

#include "xenomods/engine/fw/Document.hpp"
#include "xenomods/engine/mm/mtl/RTTI.hpp"

#if XENOMODS_NEW_ENGINE
namespace game {

	// move these out of here?

	class BehaviorComponent {
	   public:
		virtual mm::mtl::RTTI* getRTTI() const;
	};

	class BehaviorPc : BehaviorComponent {
	   public:
		static mm::mtl::RTTI m_rtti;
	};

	class ActorAccessor {
	   public:
		ActorAccessor(const fw::Document& doc, unsigned int objHandle);

		void* getPropertyComponent() const;
		BehaviorComponent* getBehaviorComponent() const;

		bool isValid() const;
	};

} // namespace game
#endif