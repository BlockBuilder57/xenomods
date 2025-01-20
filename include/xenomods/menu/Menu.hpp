// Created by block on 5/28/23.

#pragma once

#include <magic_enum.hpp>
#include <xenomods/HidInput.hpp>

#include "MenuLog.hpp"
#include "Section.hpp"

namespace xenomods {

	class Menu {
	   private:
		bool isOpen { false };
		bool logOpen { false };

		std::vector<Section*> sections {};
		std::vector<void(*)()> callbacks {};
		std::vector<void(*)()> backgroundCallbacks {};

	   public:
		void Initialize();

		void Update(HidInput* input);
		static void Render();

		void Toggle() {
			isOpen = !isOpen;
		};
		bool IsOpen() {
			return isOpen;
		};

		void ToggleLog() {
			logOpen = !logOpen;
		};

		enum class Theme {
			Auto = 0,
			Titans,
			Alrest,
			Aionios,
			ImGuiDark,
			ImGuiLight,
			ImGuiClassic,
		};

		Theme SetTheme(Theme theme);

		Section* FindSection(const std::string& key);
		Section* RegisterSection(const std::string& key, const std::string& display);
		void RegisterRenderCallback(void(*func)(), bool foregroundOnly);

		friend class Section;

		MenuLog Log {};
	};

	extern Menu* g_Menu;

} // namespace xenomods

template<>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<xenomods::Menu::Theme>(xenomods::Menu::Theme value) noexcept {
	// clang-format off
	switch (value) {
		using enum xenomods::Menu::Theme;

		case ImGuiDark: return "Dear ImGui Dark";
		case ImGuiLight: return "Dear ImGui Light";
		case ImGuiClassic: return "Dear ImGui Classic";
	}
	// clang-format on
	return default_tag;
}
