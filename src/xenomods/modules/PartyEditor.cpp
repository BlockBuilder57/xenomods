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
	gf::RQ_SETUP_PARTY PartyEditor::PartySetup {};
	int PartyEditor::PartyIdx {};
	int PartyEditor::TeamIdx {};

	OptionBase* optDriver {};
	OptionBase* optCurBlade {};
	OptionBase* optBlades[3] {};

	OptionBase* optTornaLead {};
	OptionBase* optTornaRear1 {};
	OptionBase* optTornaRear2 {};
#elif XENOMODS_CODENAME(bfsw)
	game::PcID PartyEditor::PartySetup[7] = {};
	game::PcID PartyEditor::CurrentStatus = game::PcID::None;
	game::DataUtil::AddExpApSpInfo PartyEditor::AddInfo = {};

	game::DataCharaStatus tempStatus;

	OptionBase* optStatusLevel = {};
	OptionBase* optStatusExpLevel = {};
	OptionBase* optStatusExpTotal = {};
	OptionBase* optStatusAffCoins = {};
	OptionBase* optStatusStatHp = {};
	OptionBase* optStatusStatStrength = {};
	OptionBase* optStatusStatAgility = {};
	OptionBase* optStatusStatEther = {};
#endif

#if XENOMODS_OLD_ENGINE
	void MenuGetParty() {
		gf::GfGameParty::getCurrentPartyInfo(PartyEditor::PartySetup);
		dbgutil::dumpMemory(&PartyEditor::PartySetup, sizeof(gf::RQ_SETUP_PARTY), XENOMODS_CODENAME_STR "_party.dump");
		PartyEditor::PartyIdx = 0;
	}

	void MenuApplyParty() {
		gf::GfReqCommand::reqChangeParty(PartyEditor::PartySetup);
	}

	void MenuChangePartyIndex() {
		PartyEditor::PartyIdx = std::clamp(PartyEditor::PartyIdx, 0, 9);

		gf::RQ_SETUP_PARTY_basegame* base = &PartyEditor::PartySetup.base[PartyEditor::PartyIdx];

		optDriver->SetValuePtr(&base->driver);
		optCurBlade->SetValuePtr(&base->activeBladeIdx);
		for (int i = 0; i < 3; i++)
			optBlades[i]->SetValuePtr(&base->blades[i]);
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

	void MenuChangePcID() {
		PartyEditor::CurrentStatus = (game::PcID)std::clamp((int)PartyEditor::CurrentStatus, 0, (int)game::PcID::Count);

		game::DataPc* member = game::DataUtil::getDataPc(*xenomods::DocumentPtr, static_cast<unsigned short>(PartyEditor::CurrentStatus));

		if (member == nullptr)
		{
			optStatusLevel->SetValuePtr(&tempStatus.level);
			optStatusExpLevel->SetValuePtr(&tempStatus.expLevel);
			optStatusExpTotal->SetValuePtr(&tempStatus.expTotal);
			optStatusAffCoins->SetValuePtr(&tempStatus.affinityCoins);
			optStatusStatHp->SetValuePtr(&tempStatus.baseHp);
			optStatusStatStrength->SetValuePtr(&tempStatus.baseStrength);
			optStatusStatAgility->SetValuePtr(&tempStatus.baseAgility);
			optStatusStatEther->SetValuePtr(&tempStatus.baseEther);
			return;
		}

		optStatusLevel->SetValuePtr(&member->status.level);
		optStatusExpLevel->SetValuePtr(&member->status.expLevel);
		optStatusExpTotal->SetValuePtr(&member->status.expTotal);
		optStatusAffCoins->SetValuePtr(&member->status.affinityCoins);
		optStatusStatHp->SetValuePtr(&member->status.baseHp);
		optStatusStatStrength->SetValuePtr(&member->status.baseStrength);
		optStatusStatAgility->SetValuePtr(&member->status.baseAgility);
		optStatusStatEther->SetValuePtr(&member->status.baseEther);
	}

	/*void MenuLoadCharaStatus() {
		game::DataPc* member = game::DataUtil::getDataPc(*xenomods::DocumentPtr, static_cast<unsigned short>(PartyEditor::CurrentStatus));
		dbgutil::loadMemory(&member->status, fmt::format("sd:/config/xenomods/memdump/DataCharaStatus - {}.dump", PartyEditor::CurrentStatus));
	}

	void MenuDumpAllCharaStatuses() {
		for(unsigned short i = 1; i <= 15; i++) {
			game::DataPc* member = game::DataUtil::getDataPc(*DocumentPtr, i);
			game::PcID pcid = static_cast<game::PcID>(i);
			std::string filename = fmt::format("DataCharaStatus - {}.dump", pcid);
			dbgutil::dumpMemory(&member->status, sizeof(game::DataCharaStatus), filename.c_str());
		}
	}*/

	void MenuAddExperience() {
		if (PartyEditor::AddInfo.pcid > game::PcID::None && PartyEditor::AddInfo.pcid < game::PcID::Count)
			game::DataUtil::addExpApSp(*xenomods::DocumentPtr, PartyEditor::AddInfo, false);
		else
			xenomods::g_Logger->ToastWarning("addexp", "Invalid PC selected");
	}
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
					section = baseSection->RegisterSection(std::string(STRINGIFY(PartyEditor)) + "_base", "Party...");
				else
					skipTorna = true;

				section->RegisterOption<int>(PartyIdx, "Index", &MenuChangePartyIndex);
				gf::RQ_SETUP_PARTY_basegame* base = &PartyEditor::PartySetup.base[0];
				optDriver = section->RegisterOption<std::int32_t>(base->driver, "Driver");
				optCurBlade = section->RegisterOption<std::int32_t>(base->activeBladeIdx, "Active Blade Index");
				for (int i = 0; i < 3; i++)
					optBlades[i] = section->RegisterOption<std::int16_t>(base->blades[i], "Blade " + std::to_string(i));
				section->RegisterOption<void>("Apply Party", &MenuApplyParty);
				section->RegisterOption<void>("Get Party", &MenuGetParty);
			}

			if (!skipTorna)
			{
				if(version::RuntimeGame() != version::GameType::IRA)
					section = baseSection->RegisterSection(std::string(STRINGIFY(PartyEditor)) + "_ira", "Teams...");

				section->RegisterOption<int>(TeamIdx, "Index", &MenuChangeTeamIndex);
				gf::RQ_SETUP_PARTY_torna* ira = &PartyEditor::PartySetup.ira[0];
				optTornaLead = section->RegisterOption<std::int16_t>(ira->lead, "Vanguard");
				optTornaRear1 = section->RegisterOption<std::int16_t>(ira->rear1, "Rear Guard 1");
				optTornaRear2 = section->RegisterOption<std::int16_t>(ira->rear2, "Rear Guard 2");
				section->RegisterOption<void>("Apply Teams", &MenuApplyParty);
				section->RegisterOption<void>("Get Teams", &MenuGetParty);
			}
#elif XENOMODS_CODENAME(bfsw)
			{
				auto order = baseSection->RegisterSection(std::string(STRINGIFY(PartyEditor)) + "_order", "Party Order...");
				order->RegisterOption<game::PcID>(PartySetup[0], "PC 1");
				order->RegisterOption<game::PcID>(PartySetup[1], "PC 2");
				order->RegisterOption<game::PcID>(PartySetup[2], "PC 3");
				order->RegisterOption<game::PcID>(PartySetup[3], "PC 4");
				order->RegisterOption<game::PcID>(PartySetup[4], "PC 5");
				order->RegisterOption<game::PcID>(PartySetup[5], "PC 6");
				order->RegisterOption<game::PcID>(PartySetup[6], "PC 7");
				order->RegisterOption<void>("Apply Party", &MenuApplyParty);
				order->RegisterOption<void>("Get Party", &MenuGetParty);
			}

			{
				auto status = baseSection->RegisterSection(std::string(STRINGIFY(PartyEditor)) + "_status", "PC Status...");
				//status->RegisterOption<void>("Dump all chara statuses", &MenuDumpAllCharaStatuses);
				//status->RegisterOption<void>("Load current chara status", &MenuLoadCharaStatus);
				status->RegisterOption<game::PcID>(CurrentStatus, "Party Member", &MenuChangePcID);
				optStatusLevel = status->RegisterOption<int>(tempStatus.level, "Level");
				optStatusExpLevel = status->RegisterOption<unsigned int>(tempStatus.expLevel, "EXP (cur level)");
				optStatusExpTotal = status->RegisterOption<unsigned int>(tempStatus.expTotal, "EXP (total)");
				optStatusAffCoins = status->RegisterOption<unsigned int>(tempStatus.affinityCoins, "Affinity Coins");
				optStatusStatHp = status->RegisterOption<unsigned int>(tempStatus.baseHp, "Base HP");
				optStatusStatStrength = status->RegisterOption<unsigned short>(tempStatus.baseStrength, "Base Strength");
				optStatusStatAgility = status->RegisterOption<unsigned short>(tempStatus.baseAgility, "Base Agility");
				optStatusStatEther = status->RegisterOption<unsigned short>(tempStatus.baseEther, "Base Ether");
			}

			{
				auto exp = baseSection->RegisterSection(std::string(STRINGIFY(PartyEditor)) + "_exp", "Experience...");
				exp->RegisterOption<game::PcID>(AddInfo.pcid, "Party Member");
				exp->RegisterOption<int>(AddInfo.exp, "EXP");
				exp->RegisterOption<int>(AddInfo.ap, "AP");
				exp->RegisterOption<int>(AddInfo.sp, "SP");
				exp->RegisterOption<bool>(AddInfo.showOnUI, "Show UI alerts");
				exp->RegisterOption<void>("Add Experience", &MenuAddExperience);
			}
#endif
		}
	}

#if !XENOMODS_CODENAME(bf3)
	XENOMODS_REGISTER_MODULE(PartyEditor);
#endif

} // namespace xenomods
