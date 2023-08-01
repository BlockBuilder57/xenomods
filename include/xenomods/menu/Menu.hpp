// Created by block on 5/28/23.

#pragma once

#include <xenomods/HidInput.hpp>

#include "Section.hpp"

namespace xenomods {

	class Menu {
	   private:
		bool isOpen { false };

		std::vector<Section*> sections {};
		std::vector<void(*)()> callbacks {};

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

		Section* FindSection(const std::string& key);
		Section* RegisterSection(const std::string& key, const std::string& display);
		void RegisterRenderCallback(void(*func)());

		friend class Section;
	};

	extern Menu* g_Menu;

} // namespace xenomods