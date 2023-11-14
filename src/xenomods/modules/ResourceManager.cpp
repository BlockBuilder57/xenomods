// Created by block on 7/4/23.

#include "ResourceManager.hpp"

#include "xenomods/engine/bdat/Bdat.hpp"
#include "xenomods/engine/game/DocAccessor.hpp"
#include "xenomods/engine/game/Utils.hpp"
#include "xenomods/engine/gf/Data.hpp"
#include "xenomods/engine/gf/SaveGame.hpp"
#include "xenomods/engine/gf/Util.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"

#if !XENOMODS_CODENAME(bf3)
namespace xenomods {

	unsigned short ResourceManager::ItemId = 0;
	unsigned short ResourceManager::ItemCount = 1;

	mm::mtl::FixStr<256> itemName {};
	mm::mtl::FixStr<256> itemHelp {};
	mm::mtl::FixStr<256> itemInventory {};
#if XENOMODS_OLD_ENGINE
	ui::UIStr uiStr {};
#endif

	void DrawItems() {
		const unsigned short u16_one = 1;
		const unsigned short u16_hundred = 100;

#if XENOMODS_OLD_ENGINE
		// gets the number of the item in the inventory
		auto recalculateInventory = [&](int itemType) {
			int numberInInventory = gf::GfMenuItemUtil::getItemNum(itemType, ResourceManager::ItemId);

			if(numberInInventory > 0)
				itemInventory.set(fmt::format("Num in inventory: {}", numberInInventory));
			else
				itemInventory.set("Not in inventory");
		};
#endif

		ImGui::PushItemWidth(100.f);

		ImGui::InputScalar("ID", ImGuiDataType_U16, &ResourceManager::ItemId, &u16_one, &u16_hundred, "%u");
		if(ImGui::IsItemDeactivatedAfterEdit()) {
#if XENOMODS_OLD_ENGINE
			int itemType = gf::GfDataItem::getItemType(ResourceManager::ItemId);

			// sets item name
			// formats as "[category] - [name]"
			std::string bdatSheetName = "";
			Bdat::BdatSheet* pBdat = reinterpret_cast<Bdat::BdatSheet*>(gf::GfMenuBdat::getItemDataFP(itemType, ResourceManager::ItemId));
			if (pBdat != nullptr)
				bdatSheetName = reinterpret_cast<char*>(reinterpret_cast<char*>(pBdat) + pBdat->nameOffset);

			gf::GfMenuItemUtil::getItemName(itemType, ResourceManager::ItemId, uiStr);
			if (uiStr.c_str() != nullptr) {
				if (bdatSheetName.empty())
					itemName.set(uiStr.c_str());
				else {
					itemName.set(fmt::format("{} ({})", uiStr.c_str(), bdatSheetName));
				}
				uiStr.release();
			}
			else {
				itemName.set("<no item>");
			}

			// sets description
			itemHelp.set(gf::GfDataItem::getCaption(ResourceManager::ItemId));

			recalculateInventory(itemType);
#elif XENOMODS_CODENAME(bfsw)
			game::DataUtil::getItemName(*xenomods::DocumentPtr, ResourceManager::ItemId, itemName);
			game::DataUtil::getItemHelp(*xenomods::DocumentPtr, ResourceManager::ItemId, itemHelp, 0);

			game::DataItem* dataItem = game::DataUtil::getDataItem(*xenomods::DocumentPtr);
			if(dataItem != nullptr) {
				game::DataItem::DataCommon* item = dataItem->getItem(ResourceManager::ItemId);
				if(item != nullptr) {
					if(item->isInInventory)
						itemInventory.set(fmt::format("Num in inventory: {}", item->stackCount));
					else
						itemInventory.set("Not in inventory");
				}
			}
#endif
		}
		ImGui::SameLine();
		if(ImGui::InputScalar("Count", ImGuiDataType_U16, &ResourceManager::ItemCount, &u16_one, nullptr, "%u"))
			ResourceManager::ItemCount = std::clamp((int)ResourceManager::ItemCount, 1, 99);

		ImGui::PopItemWidth();

#if XENOMODS_OLD_ENGINE
		if (ImGui::SmallButton("*10"))
			ResourceManager::ItemId *= 10;
		ImGui::SameLine();
		if (ImGui::SmallButton("/10"))
			ResourceManager::ItemId /= 10;
#endif

		if(itemName.buffer[0] != '\0')
			ImGui::TextUnformatted(itemName.buffer);
		if(itemHelp.buffer[0] != '\0')
			ImGui::TextUnformatted(itemHelp.buffer);
		if(itemInventory.buffer[0] != '\0')
			ImGui::TextUnformatted(itemInventory.buffer);

		if(ImGui::Button("Give Item")) {
#if XENOMODS_OLD_ENGINE
			gf::GfItemUtil::addItemBox(ResourceManager::ItemId, ResourceManager::ItemCount);
			int itemType = gf::GfDataItem::getItemType(ResourceManager::ItemId);
			recalculateInventory(itemType);
#elif XENOMODS_CODENAME(bfsw)
			game::DataUtil::addItem(*xenomods::DocumentPtr, ResourceManager::ItemId, ResourceManager::ItemCount, true, true, false);
#endif
		}
	}

	void ResourceManager::MenuSection() {
		const float moneyWidth = 80.f;

#if XENOMODS_OLD_ENGINE
		auto save = gf::GfDataStore::getDataGame();
		if (save != nullptr) {
			ImGui::PushItemWidth(moneyWidth);
			ImGui::DragScalar("Money", ImGuiDataType_U32, &save->money, 100.f);
			ImGui::PopItemWidth();
		}
#elif XENOMODS_CODENAME(bfsw)
		// everything here needs the document
		if(xenomods::DocumentPtr == nullptr)
			return;

		game::DataManager* dataManager = game::DocAccessor::GetFromXenomodsDocument()->getDataManager();

		if (dataManager != nullptr) {
			ImGui::PushItemWidth(moneyWidth);
			ImGui::DragScalar("Money", ImGuiDataType_U32, &dataManager->dataGame.dataGameSave.money, 100.f);
			ImGui::PopItemWidth();
		}
#endif
		if(ImGui::CollapsingHeader("Item Giver")) {
			DrawItems();
		}
	}

	void ResourceManager::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up resource manager...");

		auto modules = g_Menu->FindSection("modules");
		if(modules != nullptr) {
			auto section = modules->RegisterSection(STRINGIFY(ResourceManager), "Resource Manager");
			section->RegisterRenderCallback(&MenuSection);
		}
	}

	XENOMODS_REGISTER_MODULE(ResourceManager)

} // namespace xenomods
#endif
