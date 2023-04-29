#pragma once

#include "xenomods/Utils.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"
#include "xenomods/engine/mm/mtl/RTTI.hpp"

namespace gf {

	class GfMoveJumpControler {
	   public:
		struct Packet { // we only care abt stuff here
			mm::Vec3 velSomething;
		};
	};

	enum AnimLayer : unsigned int {
		Default
	};

	/**
	 * Placeholder for Common Animation StateMachine stuff
	 */
	class GfComAsm {
		// TODO: figure this garbage out
		INSERT_PADDING_BYTES(392);

		/**
		 * Set a float variable for this ASM
		 * \param name Name of the variable to set
		 * \param value The value to set that variable to
		 */
		void setVariable(const char* name, float value);

		/**
		 * Get a float variable
		 * \param name Ahem
		 * \param result_value Pointer to where the result should be stored.
		 */
		void getVariable(const char* name, float* result_value);

		char* getStateName(AnimLayer layer);
	};

	struct GfComPropertyPc {

		enum class Flags : std::uint32_t {
			Idle = BIT(2),
			OnWall = BIT(5),
			WallAdsorption = BIT(6),
			AutoRun = BIT(8),
			InAir = BIT(9),
		};

		INSERT_PADDING_BYTES(0x58);
		mm::Vec2 inputReal;
		INSERT_PADDING_BYTES(0x8);
		mm::Vec2 inputDupe;
		INSERT_PADDING_BYTES(0x8);
		mm::Vec3 velocityWish;
		mm::Vec3 velocityActual;
		INSERT_PADDING_BYTES(0x288);
		unsigned int flags;

		static mm::mtl::RTTI m_rtti;
		virtual mm::mtl::RTTI* getRTTI() const;
	};

	struct GfComBehaviorPc {

	};

	struct MsgObjectGetVelocity {
		INSERT_PADDING_BYTES(16);
		mm::Vec3 velocity;
	};

	namespace pc {

		struct FallDamagePlugin { /* Player controller fall damage */
			void* vtable;
			void* pluginPtr;
			void* attachObj;
			float fallMaxHeight;
			float probablyAnotherFloat;
			void* alsoTheAttachObjMaybe;
		};

	} // namespace pc

	struct PlayerCameraTarget {
		INSERT_PADDING_BYTES(32);
		mm::Vec3 moverPos;
		INSERT_PADDING_BYTES(22);
		bool inAir;
		bool bool0x43;
		bool aboveWalkableGround;
		bool inBattleCamera;
		bool groundSwimmable;
		bool bool0x47;
		float surfaceHeight;
		float maybeHeight2;
		mm::Vec3 surfaceNormal;
		float maybeHeight3;
		mm::Vec3 lowerVector;
	};

} // namespace gf
