#include "BdatRandomizer.hpp"

#include <xenomods/DebugWrappers.hpp>
#include <xenomods/HidInput.hpp>
#include <xenomods/Logger.hpp>
#include <xenomods/Utils.hpp>
#include <skylaunch/hookng/Hooks.hpp>

#include "xenomods/engine/bdat/Bdat.hpp"
#include "xenomods/engine/ml/Rand.hpp"
#include "xenomods/stuff/utils/util.hpp"
#include "nn/oe.h"

namespace {

	struct RandomizerOverride : xenomods::BdatOverrideBase {
		using enum xenomods::BdatRandomizer::BdatMSScrambleType;

		[[nodiscard]] bool IsApplicable(SheetData& sheet) const override {
			return xenomods::BdatRandomizer::msScrambleType != Off &&
				   Bdat::getMember(sheet.buffer, "name") &&
				   Bdat::getMember(sheet.buffer, "style");
		}

		void operator()(Access& access) override {
			if(access.sheet.member != "name")
				return;

			switch(xenomods::BdatRandomizer::msScrambleType) {
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

namespace xenomods {

	BdatRandomizer::BdatMSScrambleType BdatRandomizer::msScrambleType = BdatRandomizer::BdatMSScrambleType::Off;

	void BdatRandomizer::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up Bdat randomizer...");

		BdatOverride::RegisterCallback(&MsOverride());
	}

	void BdatRandomizer::Update() {
		if(GetPlayer(2)->InputDownStrict(Keybind::BDAT_MSSCRAMBLETYPE_SWITCH)) {
			reinterpret_cast<std::underlying_type_t<decltype(msScrambleType)>&>(msScrambleType) += 1;

			if(msScrambleType >= BdatMSScrambleType::Count)
				msScrambleType = BdatMSScrambleType::Off;

			g_Logger->LogInfo("Bdat text scramble type set to {}", msScrambleType);
		}
	}

	XENOMODS_REGISTER_MODULE(BdatRandomizer);

} // namespace xenomods
