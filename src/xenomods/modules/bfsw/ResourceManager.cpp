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
	mm::mtl::FixStr<256> itemInventory {};

	void ResourceManager::MenuSection() {
		// everything here needs the document
		if (xenomods::DocumentPtr == nullptr)
			return;

		const unsigned short u16_one = 1;
		const unsigned short u16_hundred = 100;

		ImGui::PushItemWidth(100.f);

		ImGui::InputScalar("ID", ImGuiDataType_U16, &ItemId, &u16_one, &u16_hundred, "%u");
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			game::DataUtil::getItemName(*xenomods::DocumentPtr, ResourceManager::ItemId, itemName);
			game::DataUtil::getItemHelp(*xenomods::DocumentPtr, ResourceManager::ItemId, itemHelp, 0);

			game::DataItem* dataItem = game::DataUtil::getDataItem(*xenomods::DocumentPtr);
			if (dataItem != nullptr)
			{
				game::DataItem::DataCommon* item = dataItem->getItem(ResourceManager::ItemId);
				if (item != nullptr)
				{
					if (item->isInInventory)
						itemInventory.set(fmt::format("Num in inventory: {}", item->stackCount));
					else
						itemInventory.set("Not in inventory");
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::InputScalar("Count", ImGuiDataType_U16, &ItemCount, &u16_one, nullptr, "%u"))
			ItemCount = std::clamp((int)ItemCount, 1, 99);

		ImGui::PopItemWidth();

		if (itemName.buffer[0] != '\0')
			ImGui::Text("%s", itemName.buffer);
		if (itemHelp.buffer[0] != '\0')
			ImGui::Text("%s", itemHelp.buffer);
		if (itemInventory.buffer[0] != '\0')
			ImGui::Text("%s", itemInventory.buffer);

		if (ImGui::Button("Give Item"))
			game::DataUtil::addItem(*xenomods::DocumentPtr, ItemId, ItemCount, true, true, false);
	}

	void ResourceManager::Initialize() {
		UpdatableModule::Initialize();
		g_Logger->LogDebug("Setting up resource manager...");

		auto modules = g_Menu->FindSection("modules");
		if (modules != nullptr) {
			auto section = modules->RegisterSection(STRINGIFY(ResourceManager), "Resource Manager");
			section->RegisterRenderCallback(&MenuSection);
		}
	}

	XENOMODS_REGISTER_MODULE(ResourceManager)

} // namespace xenomods
#endif
