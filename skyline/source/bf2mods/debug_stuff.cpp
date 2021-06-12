#include "debug_stuff.hpp"

#include "bf2logger.h"
#include "bf2mods/stuff/utils/util.hpp"
#include "bf2mods/utils.hpp"
#include "plugin.hpp"
#include "skyline/logger/Logger.hpp"

//#include <bf2mods/prettyprinter.hpp>

#include <bf2mods/gf/misc.h>

#include <bf2mods/mm/math_types.hpp>
#include <bf2mods/stuff/utils/debug_util.hpp>
#include <map>

namespace fw::debug {

	bool (*drawAxis)(const mm::Mat44* transform, float scale);
	bool (*drawFont)(int x, int y, const mm::Col4* color, const char* fmt, ...);
	bool (*drawFontGetWidth)(const char* fmt, ...);
	bool (*drawFontGetHeight)(const char* fmt, ...);
	void (*drawCompareZ)(bool compare);

} // namespace fw::debug

namespace mm {

	namespace MMStdBase {

		GENERATE_SYM_HOOK(mmAssert, "_ZN2mm9MMStdBase8mmAssertEPKcS2_j", void, const char* expr, const char* file, unsigned line) {
			bf2mods::g_Logger->LogFatal("Caught Assert!!! Expr \"%s\" (%s : %d)", expr, file, line);
			mmAssertBak(expr, file, line);
		}

	} // namespace MMStdBase

} // namespace mm

namespace gf {

	void (*GfComAsm_setVariable)(gf::GfComAsm* this_pointer, const char* name, float value);
	void (*GfComAsm_getVariable)(gf::GfComAsm* this_pointer, const char* name, float* result_value);
	char* (*GfComAsm_getStateName)(gf::GfComAsm* this_pointer, int animLayer);

	namespace pc {

		namespace StateUtil {
			void (*setFallDamageDisable)(void* that, bool option);
			void (*setEnableGravity)(void* that, bool option);
			void (*setEnableDownIK)(void* that, bool option);
		} // namespace StateUtil

		void NormalizeMovementDeltas(gf::GfComPropertyPc* pcProperty) {
			if (bf2mods::Plugin::getSharedStatePtr()->options.game.movementSpeedMult == 1.0f)
				return;

			pcProperty->velocityDelta *= bf2mods::Plugin::getSharedStatePtr()->options.game.movementSpeedMult;

			if(pcProperty->velocityDelta.XZLengthSqu() > 8.f) {
				mm::Vec3 normalized = pcProperty->velocityDelta.XZNormalized() * 8.f * bf2mods::Plugin::getSharedStatePtr()->options.game.movementSpeedMult;
				pcProperty->velocityDelta.x = normalized.x;
				pcProperty->velocityDelta.z = normalized.z;
			}

			if(bf2mods::Plugin::getSharedStatePtr()->moonJump) {
				pcProperty->velocityActual.y = 8.f;
				//dbgutil::logStackTrace();

				//---------- this function
				//7100a663e4 ai::Hfsm::update(void const*)+c8
				//7100656290 gf::GfComBehaviorPc::update(fw::UpdateInfo const&)+454
				//71006a1070 gf::GfObjUpdate::updateComponent(gf::GfObj*, fw::UpdateInfo const&)+c4
				//7100672384 gf::GfObj::update(fw::UpdateInfo const&)+20
				//71001c4b18 fw::UpdatableGroup::update(fw::UpdateInfo const&, unsigned int)+6c
				//71001c56cc fw::UpdatableObjectManager::update(fw::UpdateInfo const&, unsigned int)+74
				//71001b0964 fw::Framework::update()+1c8
			}
		}

		GENERATE_SYM_HOOK(MoveUtilField_updateMoveJump, "_ZN2gf2pc13MoveUtilField14updateMoveJumpERKN2fw10UpdateInfoERNS_15GfComPropertyPcE", void, void* param_1, gf::GfComPropertyPc* pcProperty) {
			MoveUtilField_updateMoveJumpBak(param_1, pcProperty);
			gf::pc::NormalizeMovementDeltas(pcProperty);
		}
		GENERATE_SYM_HOOK(MoveUtilField_updateMoveRun, "_ZN2gf2pc13MoveUtilField13updateMoveRunERKN2fw10UpdateInfoERNS_15GfComPropertyPcE", void, void* param_1, gf::GfComPropertyPc* pcProperty) {
			MoveUtilField_updateMoveRunBak(param_1, pcProperty);
			gf::pc::NormalizeMovementDeltas(pcProperty);
		}
		GENERATE_SYM_HOOK(MoveUtilField_updateMoveSwim, "_ZN2gf2pc13MoveUtilField14updateMoveSwimERKN2fw10UpdateInfoERNS_15GfComPropertyPcE", void, void* param_1, gf::GfComPropertyPc* pcProperty) {
			MoveUtilField_updateMoveSwimBak(param_1, pcProperty);
			gf::pc::NormalizeMovementDeltas(pcProperty);
		}

	} // namespace pc

	namespace GfDataMap {

		// to be documented
		std::uint64_t (*getMapJumpID)(unsigned n);
		std::uint64_t (*getMapID)(unsigned n);

		GENERATE_SYM_HOOK(getMapID, "_ZN2gf9GfDataMap8getMapIDEj", std::uint64_t, unsigned int n) {
			constexpr auto _override = 1ul;
			skyline::logger::s_Instance->LogFormat("gf::GfDataMap::getMapID(n: %u), just gonna return \"%llu\"", n, _override);
			return _override;
		}

	} // namespace GfDataMap

	namespace GfBdatMaplist {

		// this hook will force the game to always load map data for the a given map

		GENERATE_SYM_HOOK(getMaplistresource, "_ZNK2gf13GfBdatMaplist18getMaplistresourceEj", char*, void* this_pointer, unsigned int n) {
			constexpr char* override_str = "ma90a";

			if(bf2mods::Plugin::getSharedStatePtr()->options.bdat.mapListResourceOverride) {
				skyline::logger::s_Instance->LogFormat("gf::GfBdatMaplist::getMaplistresource(this: %p, n: %u), just gonna return \"%s\"", this_pointer, n, override_str);
				return override_str;
			} else {
				return getMaplistresourceBak(this_pointer, n);
			}
		}

	} // namespace GfBdatMaplist

	namespace GfGameUtilSkipTravel {

		GENERATE_SYM_HOOK(isEnableSkipTravel, "_ZN2gf20GfGameUtilSkipTravel18isEnableSkipTravelEj", bool, unsigned int n) {
			skyline::logger::s_Instance->LogFormat("gf::GfGameUtilSkipTravel::isEnableSkipTravel(n: 0x%08x), just gonna return true", n);
			skyline::logger::s_Instance->LogFormat("\t\b\b\b\bMapID: 0x%08x", GfDataMap::getMapIDReplace(n));
			skyline::logger::s_Instance->LogFormat("\t\b\b\b\bMapJumpID: 0x%08x", GfDataMap::getMapJumpID(n));

			return true;
		}

	} // namespace GfGameUtilSkipTravel

	namespace GfPlayFactory {
		void (*createSkipTravel)(unsigned int mapjumpId);
	}

	namespace GfMenuObjUtil {
		int (*playSE)(SEIndex index);
	}

} // namespace gf

namespace bf2mods {

	void DoMapJump(unsigned int mapjumpId) {
		gf::GfPlayFactory::createSkipTravel(mapjumpId);
		gf::GfMenuObjUtil::playSE(gf::GfMenuObjUtil::SEIndex::mapjump);
	}

	void PlaySE(unsigned int soundEffect) {
		gf::GfMenuObjUtil::playSE((gf::GfMenuObjUtil::SEIndex)soundEffect);
	}
	void PlaySE(gf::GfMenuObjUtil::SEIndex soundEffect) {
		gf::GfMenuObjUtil::playSE(soundEffect);
	}

	void SetupDebugStuff() {
		mm::MMStdBase::mmAssertHook();

		// Load debug draw functions. Once this is setup the g_Logger can be used
		// !!!!!! TODO !!!!!! MOVE THIS INTO SEPARATE FILES PER SUBSYSTEM
		util::ResolveSymbol<decltype(fw::debug::drawAxis)>(&fw::debug::drawAxis, "_ZN2fw5debug8drawAxisERKN2mm5Mat44Ef");
		util::ResolveSymbol<decltype(fw::debug::drawFont)>(&fw::debug::drawFont, "_ZN2fw5debug8drawFontEiiRKN2mm4Col4EPKcz");
		util::ResolveSymbol<decltype(fw::debug::drawFontGetWidth)>(&fw::debug::drawFontGetWidth, "_ZN2fw5debug16drawFontGetWidthEPKcz");
		util::ResolveSymbol<decltype(fw::debug::drawFontGetHeight)>(&fw::debug::drawFontGetHeight, "_ZN2fw5debug17drawFontGetHeightEv");
		util::ResolveSymbol<decltype(fw::debug::drawCompareZ)>(&fw::debug::drawCompareZ, "_ZN2fw5debug12drawCompareZEb");

		g_Logger->LogMessage(Logger::Severity::Info, "Setting up debug stuff...");

		// Resolve some game framework symbols
		util::ResolveSymbol<decltype(gf::GfDataMap::getMapJumpID)>(&gf::GfDataMap::getMapJumpID, "_ZN2gf9GfDataMap12getMapJumpIDEj");
		util::ResolveSymbol<decltype(gf::GfDataMap::getMapID)>(&gf::GfDataMap::getMapID, "_ZN2gf9GfDataMap8getMapIDEj");

		util::ResolveSymbol<decltype(gf::GfPlayFactory::createSkipTravel)>(&gf::GfPlayFactory::createSkipTravel, "_ZN2gf13GfPlayFactory16createSkipTravelEj");
		util::ResolveSymbol<decltype(gf::GfMenuObjUtil::playSE)>(&gf::GfMenuObjUtil::playSE, "_ZN2gf13GfMenuObjUtil6playSEEj");

		// Hook a GF symbol cause why not:tm:
		//gf::GfDataMap::getMapIDHook();

		// this one is hacky but i do not give a rats fucking cock
		//gf::GfBdatMaplist::getMaplistresourceHook();
		//gf::GfGameUtilSkipTravel::isEnableSkipTravelHook();

		util::ResolveSymbol<decltype(gf::pc::StateUtil::setFallDamageDisable)>(&gf::pc::StateUtil::setFallDamageDisable, "_ZN2gf2pc9StateUtil20setFallDamageDisableERNS_15GfComBehaviorPcEb");
		util::ResolveSymbol<decltype(gf::pc::StateUtil::setEnableGravity)>(&gf::pc::StateUtil::setEnableGravity, "_ZN2gf2pc9StateUtil16setEnableGravityERNS_15GfComBehaviorPcEb");
		util::ResolveSymbol<decltype(gf::pc::StateUtil::setEnableDownIK)>(&gf::pc::StateUtil::setEnableDownIK, "_ZN2gf2pc9StateUtil15setEnableDownIKERNS_15GfComBehaviorPcEb");
		//gf::pc::AttachPlugin_updateHook();

		util::ResolveSymbol<decltype(gf::GfComAsm_getVariable)>(&gf::GfComAsm_getVariable, "_ZNK2gf8GfComAsm11getVariableEPKcPf");
		util::ResolveSymbol<decltype(gf::GfComAsm_setVariable)>(&gf::GfComAsm_setVariable, "_ZN2gf8GfComAsm11setVariableEPKcf");
		util::ResolveSymbol<decltype(gf::GfComAsm_getStateName)>(&gf::GfComAsm_getStateName, "_ZNK2gf8GfComAsm12getStateNameENS_9AnimLayerE");
		//gf::pc::StateUtilField_setAsmSpeedMoveHook();

		gf::pc::MoveUtilField_updateMoveJumpHook();
		gf::pc::MoveUtilField_updateMoveRunHook();
		gf::pc::MoveUtilField_updateMoveSwimHook();
	}

} // namespace bf2mods
