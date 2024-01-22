// Created by block on 1/1/24.

#include "CompatabilityFix.hpp"

#if XENOMODS_OLD_ENGINE

#include "xenomods/engine/bdat/Bdat.hpp"
#include "xenomods/engine/event/Objects.hpp"
#include "xenomods/engine/gf/Data.hpp"
#include "xenomods/engine/gf/Manager.hpp"
#include "xenomods/engine/gf/Party.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"

namespace {

	struct MassiveMeleeMythraFix : skylaunch::hook::Trampoline<MassiveMeleeMythraFix> {
		static char* Hook(unsigned int driverId, bool unk1, bool unk2) {
			if (!gf::GfDataDriver::isIra(driverId))
				return Orig(driverId, unk1, unk2);

			// this is *not* what they do! they use a table in the binary and match it up the proper way
			// we just use their default blade (which is an Utsuro)
			// orbs are stored in the default blade's accessory slots, so this is safe to do to check for MMM.
			unsigned char* pBdat = Bdat::getFP("CHR_Dr");
			std::uint16_t bladeId = Bdat::getValCheck(pBdat, "DefBlade1", driverId, Bdat::ValueType::kUInt16);
			return gf::GfGameUtil::getBladeModel(bladeId);

			return Orig(driverId, unk1, unk2);
		}
	};

	void DoCrystalFix(gf::GF_OBJ_HANDLE* handle = nullptr) {
		if (!gf::GfGameManager::isGameTypeIra())
			return;

		if (handle == nullptr)
			handle = gf::GfGameParty::getHandleDriverByBdatID(22);

		if (!handle->IsValid())
			return;

		char* modelName = gf::GfObjUtil::getModelResourceName(handle);
		if (modelName == nullptr)
			return;

		std::string_view nameView(modelName);
		//xenomods::g_Logger->LogDebug("testing model... {}", modelName);
		if (nameView.ends_with("pc700601") || nameView.ends_with("pc710601")) {
			//xenomods::g_Logger->LogDebug("found a Mythra ({}), changing crystal", reinterpret_cast<void*>(handle));
			gf::GfGameUtil::dispHomuraCrystal(handle, false);
		}
	}

	struct TornaMythraCrystalFix_PartyChange : skylaunch::hook::Trampoline<TornaMythraCrystalFix_PartyChange> {
		static void Hook() {
			Orig();
			//xenomods::g_Logger->LogDebug("party change");
			DoCrystalFix();
		}
	};

	// I do not like this solution.
	// However, after trying multiple different things in multiple different stages
	// of cutscene object loading, this is the only place I am reliably able to
	// get the model to change parts.

	struct TornaMythraCrystalFix_EventStream : skylaunch::hook::Trampoline<TornaMythraCrystalFix_EventStream> {
		static void Hook(event::CharaObj* this_pointer) {
			Orig(this_pointer);
			//xenomods::g_Logger->LogDebug("when the model is streaming!! {}", this_pointer->modelPath.buffer);
			DoCrystalFix(this_pointer->objHandle);
		}
	};

	struct TornaMythraCrystalFix_Event : skylaunch::hook::Trampoline<TornaMythraCrystalFix_Event> {
		static void Hook(event::CharaObj* this_pointer) {
			Orig(this_pointer);
			//xenomods::g_Logger->LogDebug("when the model is there already!! {}", this_pointer->modelPath.buffer);
			DoCrystalFix(this_pointer->objHandle);
		}
	};

}

namespace xenomods {

	void CompatabilityFix::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up compatability fixes...");

		if (version::RuntimeGame() == version::GameType::IRA || version::RuntimeVersion() >= version::SemVer::v2_0_0) {
			MassiveMeleeMythraFix::HookAt("_ZN2gf10GfGameUtil14getDriverModelEjbb");

			TornaMythraCrystalFix_PartyChange::HookAt("_ZN2gf11GfGameScene14gevChangePartyEv");
			TornaMythraCrystalFix_EventStream::HookAt(&event::CharaObj::initModelStream);
			TornaMythraCrystalFix_Event::HookAt(&event::CharaObj::initModel);
		}
	}

	void CompatabilityFix::OnMapChange(unsigned short mapId) {
		if (version::RuntimeGame() == version::GameType::IRA || version::RuntimeVersion() >= version::SemVer::v2_0_0) {
			// fixes the crystal when maps load
			DoCrystalFix();
		}
	}

	XENOMODS_REGISTER_MODULE(CompatabilityFix);

} // namespace xenomods
#endif