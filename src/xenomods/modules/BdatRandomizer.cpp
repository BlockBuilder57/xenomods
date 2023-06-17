#include "BdatRandomizer.hpp"

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

		if (detail::IsModuleRegistered(STRINGIFY(BdatOverride)))
			BdatOverride::RegisterCallback(&MsOverride());

#if !XENOMODS_CODENAME(bf3)
		auto modules = g_Menu->FindSection("modules");
		if (modules != nullptr) {
			auto section = modules->RegisterSection(STRINGIFY(BdatRandomizer), "BDAT Randomizer");
			section->RegisterOption<BdatMSScrambleType>(msScrambleType, "Scramble type");
		}
#endif
	}

#if !XENOMODS_CODENAME(bf3)
	XENOMODS_REGISTER_MODULE(BdatRandomizer);
#endif

} // namespace xenomods
