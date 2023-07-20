// Created by block on 7/4/23.

#include "ResourceManager.hpp"

#include "xenomods/engine/game/Utils.hpp"
#include "xenomods/stuff/utils/debug_util.hpp"

#if XENOMODS_CODENAME(bfsw)
namespace xenomods {

	unsigned short ResourceManager::ItemId = 0;
	unsigned short ResourceManager::ItemCount = 1;

	mm::mtl::FixStr<256> itemName {};
	mm::mtl::FixStr<256> itemHelp {};
	std::string itemInventory = "";

	void MenuChangeItemId() {
		if (xenomods::DocumentPtr == nullptr)
			return;

		game::DataUtil::getItemName(*xenomods::DocumentPtr, ResourceManager::ItemId, itemName);
		game::DataUtil::getItemHelp(*xenomods::DocumentPtr, ResourceManager::ItemId, itemHelp, 0);

		game::DataItem* dataItem = game::DataUtil::getDataItem(*xenomods::DocumentPtr);
		if (dataItem == nullptr)
			return;

		game::DataItem::DataCommon* item = dataItem->getItem(ResourceManager::ItemId);
		if (item == nullptr)
			return;

		if (item->isInInventory)
			itemInventory = fmt::format("Num in inventory: {}", item->stackCount);
		else
			itemInventory = "Not in inventory";
	}

	void MenuChangeItemCount() {
		ResourceManager::ItemCount = std::clamp((int)ResourceManager::ItemCount, 1, 99);
	}

	std::string MenuItemInfo1() {
		return std::string(itemName.buffer, itemName.m_nLen);
	};
	std::string MenuItemInfo2() {
		return std::string(itemHelp.buffer, itemHelp.m_nLen);
	};
	std::string MenuItemInfo3() {
		return itemInventory;
	};

	void MenuGiveItem() {
		if (xenomods::DocumentPtr == nullptr)
			return;

		game::DataUtil::addItem(*xenomods::DocumentPtr, ResourceManager::ItemId, 1, true, true, false);
	}

	void ResourceManager::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up resource manager...");

		auto modules = g_Menu->FindSection("modules");
		if (modules != nullptr) {
			/*auto section = modules->RegisterSection(STRINGIFY(ResourceManager), "Resource Manager");
			section->RegisterOption<unsigned short>(ItemId, "ID", &MenuChangeItemId);
			section->RegisterOption<unsigned short>(ItemCount, "Count", &MenuChangeItemCount);
			section->RegisterOption<void>("Give Item", &MenuGiveItem);
			section->RegisterTextual("Item: ", {}, &MenuItemInfo1);
			section->RegisterTextual("", {}, &MenuItemInfo2);
			section->RegisterTextual("", {}, &MenuItemInfo3);*/
		}
	}

	XENOMODS_REGISTER_MODULE(ResourceManager)

} // namespace xenomods
#endif