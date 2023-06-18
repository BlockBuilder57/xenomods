// Created by block on 6/18/23.

#include "PartyEditor.hpp"

#include "xenomods/engine/game/Utils.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"

#if XENOMODS_CODENAME(bfsw) && _DEBUG
namespace xenomods {

	game::PcID PartyEditor::CurPcID = game::PcID::None;

	void MenuLoadCharaStatus() {
		game::DataPc* member = game::DataUtil::getDataPc(*xenomods::DocumentPtr, static_cast<unsigned short>(PartyEditor::CurPcID));
		dbgutil::loadMemory(&member->status, "sd:/config/xenomods/memdump/DataCharaStatus - Dummy.dump");
	}

	void MenuDumpAllCharaStatuses() {
		for (unsigned short i = 1; i <= 15; i++) {
			game::DataPc* member = game::DataUtil::getDataPc(*xenomods::DocumentPtr, i);
			game::PcID pcid = static_cast<game::PcID>(i);
			std::string filename = fmt::format("DataCharaStatus - {}.dump", pcid);
			dbgutil::dumpMemory(&member->status, sizeof(game::DataCharaStatus), filename.c_str());
		}
	}

	void PartyEditor::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up party editor...");

		auto modules = g_Menu->FindSection("modules");
		if (modules != nullptr) {
			auto section = modules->RegisterSection(STRINGIFY(PartyEditor), "Party Editor");
			section->RegisterOption<void>("Dump all chara statuses", &MenuDumpAllCharaStatuses);
			section->RegisterOption<game::PcID>(CurPcID, "Current PC");
			section->RegisterOption<void>("Load chara status", &MenuLoadCharaStatus);
		}
	}

	XENOMODS_REGISTER_MODULE(PartyEditor);

} // namespace xenomods
#endif