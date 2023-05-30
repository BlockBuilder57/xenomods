// Created by block on 5/28/23.

#pragma once

#include <xenomods/HidInput.hpp>

#include "Option.hpp"
#include "Section.hpp"

namespace xenomods {

	class Menu {
	   private:
		static mm::Col4 COLOR_BACKGROUND;
		static mm::Col4 COLOR_TITLE;
		static mm::Col4 COLOR_SECTION;
		static mm::Col4 COLOR_OPTION;
		static mm::Col4 COLOR_TEXTUAL;
		static mm::Col4 COLOR_HIGHLIGHT;

		bool isOpen { false };
		bool drawBackground { true };

		std::vector<Section> sections {};
		Section* curSection {};

		int maxIndex {};
		int curIndex {};
		bool pressSelect {};
		bool pressBack {};

		void PollMaxIndex();

	   public:
		void Initialize();

		void Update(HidInput* input);

		void Render();

		void Toggle() {
			isOpen = !isOpen;
		};
		bool IsOpen() {
			return isOpen;
		};

		Section* FindSection(const std::string& key);
		Section* RegisterSection(const std::string& key, const std::string& display);

		friend class Section;
		friend class OptionBase;
	};

	extern Menu* g_Menu;

} // namespace xenomods