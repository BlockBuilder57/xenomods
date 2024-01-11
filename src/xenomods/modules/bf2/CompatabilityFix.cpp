// Created by block on 1/1/24.

#include "CompatabilityFix.hpp"

#if XENOMODS_OLD_ENGINE

#include "xenomods/engine/bdat/Bdat.hpp"
#include "xenomods/engine/gf/Party.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"
#include "xenomods/engine/gf/Manager.hpp"

namespace {

	struct MassiveMeleeMythraFix : skylaunch::hook::Trampoline<MassiveMeleeMythraFix> {
		static char* Hook(unsigned int driverId, bool unk1, bool unk2) {
			if (driverId < 17)
				return Orig(driverId, unk1, unk2);

			// this is *not* what they do! they use a table in the binary and match it up the proper way
			// we check if the Ira param is assigned to anything, then just use their default blade (which is an Utsuro)
			// orbs are stored in the default blade's accessory slots, so this is safe to do to check for MMM.
			unsigned char* pBdat = Bdat::getFP("CHR_Dr");
			std::uint16_t iraParam = Bdat::getValCheck(pBdat, "IraParam", driverId, Bdat::ValueType::kUInt16);
			if (iraParam > 0) {
				std::uint16_t bladeId = Bdat::getValCheck(pBdat, "DefBlade1", driverId, Bdat::ValueType::kUInt16);
				return gf::GfGameUtil::getBladeModel(bladeId);
			}

			return Orig(driverId, unk1, unk2);
		}
	};

	void DoCrystalFix() {
		gf::GF_OBJ_HANDLE* handle = gf::GfGameParty::getHandleDriverByBdatID(22);
		if (handle->IsValid() && gf::GfGameManager::isGameTypeIra()) {
			//xenomods::g_Logger->LogInfo("found Mythra ({}), changing crystal", reinterpret_cast<void*>(handle));
			gf::GfGameUtil::dispHomuraCrystal(handle, false);
		}
	}

	struct TornaMythraCrystalFix_PartyChange : skylaunch::hook::Trampoline<TornaMythraCrystalFix_PartyChange> {
		static void Hook() {
			Orig();
			DoCrystalFix();
		}
	};

}

namespace xenomods {

	void CompatabilityFix::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up compatability fixes...");

		MassiveMeleeMythraFix::HookAt("_ZN2gf10GfGameUtil14getDriverModelEjbb");
		TornaMythraCrystalFix_PartyChange::HookAt("_ZN2gf11GfGameScene14gevChangePartyEv");
	}

	void CompatabilityFix::OnMapChange(unsigned short mapId) {
		DoCrystalFix();
	}

	XENOMODS_REGISTER_MODULE(CompatabilityFix);

} // namespace xenomods
#endif