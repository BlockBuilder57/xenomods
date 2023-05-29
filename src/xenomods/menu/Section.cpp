// Created by block on 5/28/23.

#include <xenomods/DebugWrappers.hpp>

#include "xenomods/menu/Section.hpp"
#include "xenomods/menu/Menu.hpp"

namespace xenomods {

	Section::Section(const std::string& key, const std::string& display)
		: key(key),
		  display(display),
		  subsections(),
		  options(),
		  textuals() {
	}

	void Section::Update() {

	}

	void Section::PerformSelect(int index) {
		if (index < subsections.size()) {
			g_Menu->curSection = &subsections[index];
		}
		else if (index - subsections.size() < options.size()) {
			// option thing...
		}
	}

	void Section::Render(mm::Pnt<int>& pnt) {
		const int fontHeight = xenomods::debug::drawFontGetHeight();

		// a dummy option, the user will press back to go back
		xenomods::debug::drawFontFmtShadow(pnt.x, pnt.y += fontHeight, g_Menu->pressBack ? Menu::COLOR_HIGHLIGHT : Menu::COLOR_SECTION, " .. ");

		int renderNum = 0;

		for(auto& sec : subsections) {
			if(renderNum == g_Menu->curIndex)
				xenomods::debug::drawFontFmtShadow(pnt.x, pnt.y += fontHeight, g_Menu->pressSelect ? Menu::COLOR_HIGHLIGHT : Menu::COLOR_SECTION, ">{} ", sec.GetName());
			else
				xenomods::debug::drawFontFmtShadow(pnt.x, pnt.y += fontHeight, Menu::COLOR_SECTION, " {} ", sec.GetName());

			renderNum++;
		}

		for(auto& opt : options) {
			renderNum++;
		}

		for(auto& tex : textuals) {
			xenomods::debug::drawFontFmtShadow(pnt.x, pnt.y += fontHeight, tex.color.a > 0 ? tex.color : Menu::COLOR_TEXTUAL, " {} ", tex.text);
		}

	}

	void Section::AddTextual(const std::string& text, const mm::Col4& color) {
		Textual elem = { .text = text, .color = color };
		textuals.push_back(elem);
	}

} // namespace xenomods