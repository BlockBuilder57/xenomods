#include "BdatRandomizer.hpp"

#include <bf2mods/DebugWrappers.hpp>
#include <bf2mods/HidInput.hpp>
#include <bf2mods/Logger.hpp>
#include <bf2mods/Utils.hpp>
#include <skylaunch/hookng/Hooks.hpp>

#include "../State.hpp"
#include "../main.hpp"
#include "bf2mods/engine/bdat/Bdat.hpp"
#include "bf2mods/engine/ml/Rand.hpp"
#include "bf2mods/stuff/utils/util.hpp"
#include "nn/oe.h"

namespace {

	struct RandomizerOverride : bf2mods::BdatOverrideBase {
		using enum bf2mods::BdatRandomizer::BdatMSScrambleType;

		[[nodiscard]] bool IsApplicable(SheetData& sheet) const override {
			return bf2mods::BdatRandomizer::msScrambleType != Off &&
				   Bdat::getMember(sheet.buffer, "name") &&
				   Bdat::getMember(sheet.buffer, "style");
		}

		void operator()(Access& access) override {
			if(access.sheet.member != "name")
				return;

			switch(bf2mods::BdatRandomizer::msScrambleType) {
				case ScrambleIndex: {
					access.sheet.row = (ml::mtRand() % Bdat::getIdCount(access.sheet.buffer)) + Bdat::getIdTop(access.sheet.buffer);
				} break;
				case ShowSheetName: {
					auto label = Bdat::getMSLabel(access.sheet.buffer, access.sheet.row);
					access.data = reinterpret_cast<unsigned long>(label != nullptr ? label : Bdat::getSheetName(access.sheet.buffer));
				} break;
				default:
					break;
			}
		};
	};

	static auto& MsOverride() {
		static RandomizerOverride gOverride;
		return gOverride;
	}

} // namespace

namespace bf2mods {

	BdatRandomizer::BdatMSScrambleType BdatRandomizer::msScrambleType = BdatRandomizer::BdatMSScrambleType::Off;

	void BdatRandomizer::Initialize() {
		g_Logger->LogDebug("Setting up Bdat randomizer...");

		BdatOverride::RegisterCallback(&MsOverride());
	}

	void BdatRandomizer::Update() {
		if(GetPlayer(2)->InputDownStrict(Keybind::BDAT_MSSCRAMBLETYPE_SWITCH)) {
			underlying_value(msScrambleType) += 1;

			if(msScrambleType >= BdatMSScrambleType::Count)
				msScrambleType = BdatMSScrambleType::Off;

			g_Logger->LogInfo("Bdat text scramble type set to {}", msScrambleType);
		}
	}

	BF2MODS_REGISTER_MODULE(BdatRandomizer);

} // namespace bf2mods
