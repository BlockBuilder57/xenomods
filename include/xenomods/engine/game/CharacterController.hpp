// Created by block on 2/21/2023.

#pragma once

#include <xenomods/Utils.hpp>
#include <xenomods/engine/mm/MathTypes.hpp>

#include "Actor.hpp"

#if XENOMODS_CODENAME(bfsw)
namespace game {

	class CharacterController {
	   public:
		enum class Flags : std::uint32_t {
			EnableSlide = BIT(1),
			pcClimb = BIT(2),
			LandingRelated = BIT(4),
			WarpRelated = BIT(6),
			UseNavi = BIT(9),
			NaviNotFoundStraight = BIT(10),
			PassThroughTarget = BIT(11),
			SomethingVelocityRelated = BIT(17),
			AdjustWall = BIT(18),
			unknownFillout = BIT(23),
			EnableGravity = BIT(24),
			NaviArrive = BIT(28),
			VerticalOutsideArrive = BIT(30)
		};

		INSERT_PADDING_BYTES(0x10)
		ActorAccessor* actorAccessor;
		INSERT_PADDING_BYTES(0x38)
		mm::Vec3 position;
		INSERT_PADDING_BYTES(0xD4)
		mm::Vec3 velocity;
		INSERT_PADDING_BYTES(0x114)
		Flags flags;

		bool isAdsorptionSnap() const;
		bool isInAir() const;
		bool isInWater() const;
		bool isUseNavi() const;

		float getFallHeight();

		void syncFrame();

		void applyMoveVec(const mm::Vec3&, bool param_2);
		void setWarp(const mm::Vec3& pos, int usually5);
	};

} // namespace game
#endif