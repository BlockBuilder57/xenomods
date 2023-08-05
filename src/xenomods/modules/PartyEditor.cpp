// Created by block on 6/18/23.

#include "PartyEditor.hpp"

#include "helpers/ImGuiExtensions.hpp"
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
#elif XENOMODS_CODENAME(bfsw)
	game::PcID PartyEditor::PartySetup[7] = {};
	game::PcID PartyEditor::StatusCurrentPcID = game::PcID::None;
	game::DataPc* PartyEditor::StatusCurrentDataPc = nullptr;
	game::DataUtil::AddExpApSpInfo PartyEditor::AddInfo = {};
#endif

#if XENOMODS_OLD_ENGINE
	void MenuGetParty() {
		gf::GfGameParty::getCurrentPartyInfo(PartyEditor::PartySetup);
		//dbgutil::dumpMemory(&PartyEditor::PartySetup, sizeof(gf::RQ_SETUP_PARTY), XENOMODS_CODENAME_STR "_party.dump");
		PartyEditor::PartyIdx = 0;
	}

	void MenuApplyParty() {
		gf::GfReqCommand::reqChangeParty(PartyEditor::PartySetup);
	}

	void MenuChangePartyIndex() {
		PartyEditor::PartyIdx = std::clamp(PartyEditor::PartyIdx, 0, 9);

		gf::RQ_SETUP_PARTY_basegame* base = &PartyEditor::PartySetup.base[PartyEditor::PartyIdx];
	}

	void MenuChangeTeamIndex() {
		PartyEditor::TeamIdx = std::clamp(PartyEditor::TeamIdx, 0, 2);

		gf::RQ_SETUP_PARTY_torna* ira = &PartyEditor::PartySetup.ira[PartyEditor::TeamIdx];
	}

	void PartyEditor::MenuBaseParty() {
		ImGui::PushItemWidth(ImGui::GetFrameHeight() * 5.f);

		if (ImGui::Button("Apply Party"))
			MenuApplyParty();
		ImGui::SameLine();
		if (ImGui::Button("Get Party"))
			MenuGetParty();

		if (ImGui::InputInt("Index", &PartyIdx))
			PartyIdx = std::clamp(PartyIdx, 0, 9);

		gf::RQ_SETUP_PARTY_basegame* base = &PartySetup.base[PartyIdx];

		if (ImGui::InputInt("Driver", &base->driver))
			base->driver = std::clamp(base->driver, -1, 25);

		int bladeCount = 3;
		if (base->driver == 1)
			bladeCount = 6; // all have 6, but only Rex uses them all

		if (ImGui::InputInt("Active Blade Index", &base->activeBladeIdx))
			base->activeBladeIdx = std::clamp(base->activeBladeIdx, 0, bladeCount - 1);

		ImGui::Separator();

		for(int i = 0; i < bladeCount; i++) {
			const std::uint16_t u16one = 1;
			std::string name = "Blade " + std::to_string(i);
			if (ImGui::InputScalar(name.data(), ImGuiDataType_U16, &base->blades[i], &u16one))
				if (base->blades[i] > 0 && base->blades[i] <= 1000)
					base->blades[i] = 1001; // set to Pyra if "out of bounds"
			ImGui::SameLine();
			if (ImGui::Button("None"))
				base->blades[i] = 0;
		}

		ImGui::PopItemWidth();
	};

	void PartyEditor::MenuIraParty() {
		ImGui::PushItemWidth(ImGui::GetFrameHeight() * 5.f);

		if (ImGui::Button("Apply Teams"))
			MenuApplyParty();
		ImGui::SameLine();
		if (ImGui::Button("Get Teams"))
			MenuGetParty();

		if (ImGui::InputInt("Index", &TeamIdx))
			TeamIdx = std::clamp(TeamIdx, 0, 2);

		gf::RQ_SETUP_PARTY_torna* ira = &PartySetup.ira[TeamIdx];
		const std::uint16_t u16one = 1;

		ImGui::Separator();

		if (ImGui::InputScalar("Vanguard", ImGuiDataType_U16, &ira->lead, &u16one))
			ira->lead = std::clamp<std::uint16_t>(ira->lead, 0, 25);
		if (ImGui::InputScalar("Rear Guard 1", ImGuiDataType_U16, &ira->rear1, &u16one))
			ira->rear1 = std::clamp<std::uint16_t>(ira->rear1, 0, 25);
		if (ImGui::InputScalar("Rear Guard 2", ImGuiDataType_U16, &ira->rear2, &u16one))
			ira->rear2 = std::clamp<std::uint16_t>(ira->rear2, 0, 25);

		ImGui::PopItemWidth();
	}
#elif XENOMODS_CODENAME(bfsw)
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

	void PartyEditor::MenuSection() {

	}

	void PartyEditor::MenuOrder() {
		ImGui::PushItemWidth(ImGui::GetFrameHeight() * 7.f);
		imguiext::EnumComboBox("Member 1", &PartySetup[0]);
		imguiext::EnumComboBox("Member 2", &PartySetup[1]);
		imguiext::EnumComboBox("Member 3", &PartySetup[2]);
		imguiext::EnumComboBox("Member 4", &PartySetup[3]);
		imguiext::EnumComboBox("Member 5", &PartySetup[4]);
		imguiext::EnumComboBox("Member 6", &PartySetup[5]);
		imguiext::EnumComboBox("Member 7", &PartySetup[6]);
		ImGui::PopItemWidth();

		if(ImGui::Button("Load")) {
			game::DataParty* party = game::DataUtil::getDataParty(*DocumentPtr);
			if(party != nullptr) {
				// clears and sets to party
				memset(&PartyEditor::PartySetup[0], 0, sizeof(PartyEditor::PartySetup));
				memcpy(&PartyEditor::PartySetup[0], &party->members[0], sizeof(game::PcID) * party->count);
			}
		}

		ImGui::SameLine();

		if(ImGui::Button("Apply")) {
			game::DataParty* party = game::DataUtil::getDataParty(*DocumentPtr);
			if(party != nullptr) {
				std::vector<game::PcID> holder {};
				for(int i = 0; i < 7; ++i) {
					auto pcid = PartyEditor::PartySetup[i];
					if(pcid > game::PcID::None && pcid <= game::PcID::Ponspector_Tentoo)
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
				if(seqManager != nullptr) {
					auto seqStack = seqManager->getField();
					if(seqStack != nullptr) {
						// Refreshes party
						seqStack->addCommandImpl<game::SeqPartyMake>();

						// Open the party change dialog
						game::ScriptUtil::s_document = DocumentPtr;
						game::ScriptUI::openPartyChange();
					}
				}
			}
		}
	}

	void PartyEditor::MenuStatus() {
		ImGui::PushItemWidth(ImGui::GetFrameHeight() * 7.f);
		// update the datapc pointer after changing
		if(imguiext::EnumComboBox("Member", &StatusCurrentPcID))
			StatusCurrentDataPc = game::DataUtil::getDataPc(*xenomods::DocumentPtr, static_cast<unsigned short>(PartyEditor::StatusCurrentPcID));

		// don't do anything if we don't have a datapc
		if(StatusCurrentDataPc == nullptr || StatusCurrentPcID == game::PcID::None) {
			ImGui::PopItemWidth();
			return;
		}

		ImGui::InputInt("Level", &StatusCurrentDataPc->status.level);
		ImGui::DragScalar("EXP (cur level)", ImGuiDataType_U32, &StatusCurrentDataPc->status.expLevel, 1.f);
		ImGui::DragScalar("EXP (total)", ImGuiDataType_U32, &StatusCurrentDataPc->status.expTotal, 1.f);
		ImGui::DragScalar("Bonus EXP", ImGuiDataType_U32, &StatusCurrentDataPc->status.expBonus, 1.f);
		ImGui::DragScalar("Affinity Coins", ImGuiDataType_U32, &StatusCurrentDataPc->status.affinityCoins, 1.f);

		if(ImGui::CollapsingHeader("Base Stats")) {
			ImGui::DragScalar("Base HP", ImGuiDataType_U32, &StatusCurrentDataPc->status.baseHp, 1.f);
			ImGui::DragScalar("Base Strength", ImGuiDataType_U16, &StatusCurrentDataPc->status.baseStrength, 1.f);
			ImGui::DragScalar("Base Agility", ImGuiDataType_U16, &StatusCurrentDataPc->status.baseAgility, 1.f);
			ImGui::DragScalar("Base Ether", ImGuiDataType_U16, &StatusCurrentDataPc->status.baseEther, 1.f);
		}

		ImGui::PopItemWidth();
	}

	void PartyEditor::MenuAddExperience() {
		ImGui::PushItemWidth(ImGui::GetFrameHeight() * 7.f);
		imguiext::EnumComboBox("Member", &AddInfo.pcid);
		if(ImGui::Button("Clear Values"))
			AddInfo.exp = AddInfo.ap = AddInfo.sp = 0;

		ImGui::PushItemWidth(ImGui::GetFrameHeight() * 4.f);
		ImGui::DragInt("AP", &AddInfo.ap, 5.f);
		ImGui::SameLine();
		ImGui::DragInt("SP", &AddInfo.sp, 5.f);
		ImGui::DragInt("EXP", &AddInfo.exp, 5.f);
		ImGui::PopItemWidth();
		ImGui::PopItemWidth();
		ImGui::Checkbox("Show on UI", &AddInfo.showOnUI);

		if(ImGui::Button("Add"))
			game::DataUtil::addExpApSp(*xenomods::DocumentPtr, AddInfo, false);
	}
#endif

	void PartyEditor::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up party editor...");

		auto modules = g_Menu->FindSection("modules");
		if(modules != nullptr) {
			auto baseSection = modules->RegisterSection(STRINGIFY(PartyEditor), "Party Editor");
			//baseSection->RegisterRenderCallback(&MenuSection);

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

				section->RegisterRenderCallback(&MenuBaseParty);
			}

			if(!skipTorna) {
				if(version::RuntimeGame() != version::GameType::IRA)
					section = baseSection->RegisterSection(std::string(STRINGIFY(PartyEditor)) + "_ira", "Teams...");

				section->RegisterRenderCallback(&MenuIraParty);


			}
#elif XENOMODS_CODENAME(bfsw)
			auto order = baseSection->RegisterSection(std::string(STRINGIFY(PartyEditor)) + "_order", "Party Order");
			order->RegisterRenderCallback(&MenuOrder);

			auto status = baseSection->RegisterSection(std::string(STRINGIFY(PartyEditor)) + "_status", "PC Status");
			status->RegisterRenderCallback(&MenuStatus);

			auto exp = baseSection->RegisterSection(std::string(STRINGIFY(PartyEditor)) + "_addexp", "Add Experience");
			exp->RegisterRenderCallback(&MenuAddExperience);
#endif
		}
	}

#if !XENOMODS_CODENAME(bf3)
	XENOMODS_REGISTER_MODULE(PartyEditor);
#endif

} // namespace xenomods
