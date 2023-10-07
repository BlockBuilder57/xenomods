// Created by block on 8/3/23.

#pragma once

#include <imgui.h>

#include <magic_enum.hpp>
#include <string_view>

#include <xenomods/menu/Themes.hpp>

namespace xenomods::imguiext {

	template<typename T>
	bool EnumComboBox(std::string_view label, T* value) {
		const auto selected_name = magic_enum::enum_name<T>(*value);

		bool result = false;

		if(ImGui::BeginCombo(label.data(), selected_name.data())) {
			for(auto enum_value : magic_enum::enum_values<T>()) {
				const bool selected = (enum_value == *value);
				const auto name = magic_enum::enum_name(enum_value);

				if(ImGui::Selectable(name.data(), selected)) {
					*value = enum_value;
					result = true;
				}

				// meow
				if(selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		return result;
	}

} // namespace xenomods::imgui