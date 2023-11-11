#include "BdatRandomizer.hpp"

#include "xenomods/ImGuiExtensions.hpp"
#include "xenomods/engine/bdat/Bdat.hpp"
#include "xenomods/engine/ml/Rand.hpp"
#include "xenomods/stuff/utils/util.hpp"

namespace {

	struct RandomizerOverride : xenomods::BdatOverrideBase {
		using enum xenomods::BdatRandomizer::BdatMSScrambleType;

		[[nodiscard]] bool IsApplicable(SheetData& sheet) const override {
			return xenomods::BdatRandomizer::msScrambleType != Off &&
				   std::string_view(sheet.name).ends_with("_ms") &&
				   strcmp(sheet.member, "name") == 0;
		}

		void operator()(Access& access) override {
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

	void BdatRandomizer::MenuSection() {
		ImGui::PushItemWidth(ImGui::GetFrameHeight() * 9.f);
		imguiext::EnumComboBox("Scramble Type", &msScrambleType);
		ImGui::PopItemWidth();
	}

	void BdatRandomizer::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up Bdat randomizer...");

		if(detail::IsModuleRegistered(STRINGIFY(BdatOverride)))
			BdatOverride::RegisterCallback(&MsOverride());

#if !XENOMODS_CODENAME(bf3)
		auto modules = g_Menu->FindSection("modules");
		if(modules != nullptr) {
			auto section = modules->RegisterSection(STRINGIFY(BdatRandomizer), "BDAT Randomizer");
			section->RegisterRenderCallback(&MenuSection);
		}
#endif
	}

#if !XENOMODS_CODENAME(bf3)
	XENOMODS_REGISTER_MODULE(BdatRandomizer);
#endif

} // namespace xenomods
