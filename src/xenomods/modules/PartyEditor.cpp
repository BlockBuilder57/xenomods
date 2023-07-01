// Created by block on 6/18/23.

#include "PartyEditor.hpp"

#include "xenomods/engine/game/DocAccessor.hpp"
#include "xenomods/engine/game/Scripts.hpp"
#include "xenomods/engine/game/Utils.hpp"
#include "xenomods/engine/gf/AddOnContent.hpp"
#include "xenomods/engine/gf/Command.hpp"
#include "xenomods/engine/gf/Manager.hpp"
#include "xenomods/engine/gf/MenuObject.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"

namespace xenomods {

#if XENOMODS_OLD_ENGINE
	gf::RQ_SETUP_PARTY PartyEditor::PartySetup = {};
	int PartyEditor::DriverIdx = 0;
	int PartyEditor::TeamIdx = 0;

	OptionBase* optDriver = {};
	OptionBase* optCurBlade = {};
	OptionBase* optBlade1 = {};
	OptionBase* optBlade2 = {};
	OptionBase* optBlade3 = {};

	OptionBase* optTornaLead = {};
	OptionBase* optTornaRear1 = {};
	OptionBase* optTornaRear2 = {};
#elif XENOMODS_CODENAME(bfsw)
	game::PcID PartyEditor::PartySetup[7] = {};
#endif

#if XENOMODS_OLD_ENGINE
	void MenuGetParty() {
		gf::GfGameParty::getCurrentPartyInfo(PartyEditor::PartySetup);
		//dbgutil::dumpMemory(&PartyEditor::PartySetup, sizeof(gf::RQ_SETUP_PARTY), XENOMODS_CODENAME_STR "_party.dump");
		PartyEditor::DriverIdx = 0;
	}

	void MenuApplyParty() {
		gf::GfReqCommand::reqChangeParty(PartyEditor::PartySetup);
	}

	void MenuChangeDriverIndex() {
		PartyEditor::DriverIdx = std::clamp(PartyEditor::DriverIdx, 0, 9);

		gf::RQ_SETUP_PARTY_basegame* base = &PartyEditor::PartySetup.base[PartyEditor::DriverIdx];

		optDriver->SetValuePtr(&base->driver);
		optCurBlade->SetValuePtr(&base->curBlade);
		optBlade1->SetValuePtr(&base->blades[0]);
		optBlade2->SetValuePtr(&base->blades[1]);
		optBlade3->SetValuePtr(&base->blades[2]);
	}

	void MenuChangeTeamIndex() {
		PartyEditor::TeamIdx = std::clamp(PartyEditor::TeamIdx, 0, 2);

		gf::RQ_SETUP_PARTY_torna* ira = &PartyEditor::PartySetup.ira[PartyEditor::TeamIdx];

		optTornaLead->SetValuePtr(&ira->lead);
		optTornaRear1->SetValuePtr(&ira->rear1);
		optTornaRear2->SetValuePtr(&ira->rear2);
	}
#elif XENOMODS_CODENAME(bfsw)
	void MenuGetParty() {
		game::DataParty* party = game::DataUtil::getDataParty(*DocumentPtr);
		if(party == nullptr)
			return;

		// clears and sets to party
		memset(&PartyEditor::PartySetup[0], 0, sizeof(PartyEditor::PartySetup));
		memcpy(&PartyEditor::PartySetup[0], &party->members[0], sizeof(game::PcID) * party->count);
	}

	void MenuApplyParty() {
		game::DataParty* party = game::DataUtil::getDataParty(*DocumentPtr);
		if(party == nullptr)
			return;

		std::vector<game::PcID> holder {};
		for(int i = 0; i < 7; ++i) {
			auto pcid = PartyEditor::PartySetup[i];
			if(pcid > game::PcID::None && pcid < game::PcID::Count)
				holder.push_back(pcid);
		}

		if(holder.size() <= 0) {
			g_Logger->ToastError(STRINGIFY(PartyEditor), "Party size is 0! Please choose some party members.");
			return;
		}

		for(int i = 0; i < holder.size(); ++i) {
			party->members[i] = holder[i];
		}
		party->count = holder.size();

		auto seqManager = reinterpret_cast<game::DocAccessor*>(&DocumentPtr)->getSeqManager();
		if(seqManager == nullptr)
			return;

		auto seqStack = seqManager->getField();
		if(seqStack == nullptr)
			return;

		// Refreshes party
		seqStack->addCommandImpl<game::SeqPartyMake>();

		// Open the party change dialog
		game::ScriptUtil::s_document = DocumentPtr;
		game::ScriptUI::openPartyChange();
	}

	/*void MenuLoadCharaStatus() {
		game::DataPc* member = game::DataUtil::getDataPc(*xenomods::DocumentPtr, static_cast<unsigned short>(game::PcID::Shulk));
		dbgutil::loadMemory(&member->status, "sd:/config/xenomods/memdump/DataCharaStatus - Dummy.dump");
	}

	void MenuDumpAllCharaStatuses() {
		for(unsigned short i = 1; i <= 15; i++) {
			game::DataPc* member = game::DataUtil::getDataPc(*DocumentPtr, i);
			game::PcID pcid = static_cast<game::PcID>(i);
			std::string filename = fmt::format("DataCharaStatus - {}.dump", pcid);
			dbgutil::dumpMemory(&member->status, sizeof(game::DataCharaStatus), filename.c_str());
		}
	}*/
#endif

	void PartyEditor::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up party editor...");

		auto modules = g_Menu->FindSection("modules");
		if(modules != nullptr) {
			auto baseSection = modules->RegisterSection(STRINGIFY(PartyEditor), "Party Editor");

			// this is done to properly support only showing the right controls
			// for Torna, as DLC or as standalone
			// (ie on <2.0.0 we don't make a base game section)
			auto section = baseSection;
#if XENOMODS_OLD_ENGINE
			bool skipTorna = false;

			if(version::RuntimeGame() == version::GameType::BF2) {
				if(version::RuntimeVersion() >= version::SemVer::v2_0_0 && gf::GfDataAoc::getContentVersion(gf::AOC_TYPE::Torna) > -1)
					section = baseSection->RegisterSection(std::string(STRINGIFY(PartyEditor)) + "_base", "Base game");
				else
					skipTorna = true;

				section->RegisterOption<int>(DriverIdx, "Driver Index", &MenuChangeDriverIndex);
				gf::RQ_SETUP_PARTY_basegame* base = &PartyEditor::PartySetup.base[0];
				optDriver = section->RegisterOption<std::int32_t>(base->driver, "Driver");
				optCurBlade = section->RegisterOption<std::int32_t>(base->curBlade, "Cur Blade");
				optBlade1 = section->RegisterOption<std::int16_t>(base->blades[0], "Blade 1");
				optBlade2 = section->RegisterOption<std::int16_t>(base->blades[1], "Blade 2");
				optBlade3 = section->RegisterOption<std::int16_t>(base->blades[2], "Blade 3");
				section->RegisterOption<void>("Apply Party", &MenuApplyParty);
				section->RegisterOption<void>("Get Party", &MenuGetParty);
			}

			if (!skipTorna)
			{
				if(version::RuntimeGame() != version::GameType::IRA)
					section = baseSection->RegisterSection(std::string(STRINGIFY(PartyEditor)) + "_ira", "Torna");

				section->RegisterOption<int>(TeamIdx, "Team Index", &MenuChangeTeamIndex);
				gf::RQ_SETUP_PARTY_torna* ira = &PartyEditor::PartySetup.ira[0];
				optTornaLead = section->RegisterOption<std::int16_t>(ira->lead, "Vanguard");
				optTornaRear1 = section->RegisterOption<std::int16_t>(ira->rear1, "Rearguard 1");
				optTornaRear2 = section->RegisterOption<std::int16_t>(ira->rear2, "Rearguard 2");
				section->RegisterOption<void>("Apply Party", &MenuApplyParty);
				section->RegisterOption<void>("Get Party", &MenuGetParty);
			}

#elif XENOMODS_CODENAME(bfsw)
			//section->RegisterOption<void>("Dump all chara statuses", &MenuDumpAllCharaStatuses);
			//section->RegisterOption<void>("Load chara status", &MenuLoadCharaStatus);
			section->RegisterOption<game::PcID>(PartySetup[0], "PC 1");
			section->RegisterOption<game::PcID>(PartySetup[1], "PC 2");
			section->RegisterOption<game::PcID>(PartySetup[2], "PC 3");
			section->RegisterOption<game::PcID>(PartySetup[3], "PC 4");
			section->RegisterOption<game::PcID>(PartySetup[4], "PC 5");
			section->RegisterOption<game::PcID>(PartySetup[5], "PC 6");
			section->RegisterOption<game::PcID>(PartySetup[6], "PC 7");
			section->RegisterOption<void>("Apply Party", &MenuApplyParty);
			section->RegisterOption<void>("Get Party", &MenuGetParty);
#endif
		}
	}

#if !XENOMODS_CODENAME(bf3)
	XENOMODS_REGISTER_MODULE(PartyEditor);
#endif

} // namespace xenomods
