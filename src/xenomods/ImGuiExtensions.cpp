// Created by block on 8/28/23.

#include <xenomods/ImGuiExtensions.hpp>

namespace xenomods::imguiext {

	const char* styleNames[] = { "Titans", "Alrest", "Aionios", "Dear ImGui Dark", "Dear ImGui Light", "Dear ImGui Classic\0" };

	bool ShowStyleSelector(const char* label)
	{
		static int style_idx = -1;
		if (ImGui::Combo(label, &style_idx, styleNames, std::size(styleNames)))
		{
			switch (style_idx)
			{
				case 0: ImGuiStyleColorsXB1(); break;
				case 1: ImGuiStyleColorsXB2(); break;
				case 2: ImGuiStyleColorsXB3(); break;
				case 3: ImGui::StyleColorsDark(); break;
				case 4: ImGui::StyleColorsLight(); break;
				case 5: ImGui::StyleColorsClassic(); break;
				default: ImGui::StyleColorsDark(); break;
			}
			return true;
		}
		return false;
	}

}