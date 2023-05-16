// Created by block on 2/21/2023.

#pragma once

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
		void* getPropertyComponent() const;
		BehaviorComponent* getBehaviorComponent() const;
	};

} // namespace game
#endif