// Created by block on 5/28/23.

#include <xenomods/DebugWrappers.hpp>
#include <xenomods/Logger.hpp>

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

	void Section::Update(HidInput* input) {
		if (g_Menu->curIndex < subsections.size()) {
			curOption = nullptr;
		}
		else if (g_Menu->curIndex - subsections.size() < options.size()) {
			if (curOption != nullptr) {
				if (curOption->IsSelected()) {
					if (curOption->Update(input))
						curOption->Callback();
				}
				else
					curOption = nullptr;
			}
		}
	}

	void Section::PerformSelect() {
		if (g_Menu->curIndex < subsections.size()) {
			g_Menu->curSection = &subsections[g_Menu->curIndex];
		}
		else if (g_Menu->curIndex - subsections.size() < options.size()) {
			curOption = options[g_Menu->curIndex - subsections.size()];
			curOption->SetSelected();
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

		for(auto* opt : options) {
			if(renderNum == g_Menu->curIndex)
				xenomods::debug::drawFontFmtShadow(pnt.x, pnt.y += fontHeight, opt->IsSelected() ? Menu::COLOR_HIGHLIGHT : Menu::COLOR_OPTION, ">{} ", opt->String());
			else
				xenomods::debug::drawFontFmtShadow(pnt.x, pnt.y += fontHeight, Menu::COLOR_OPTION, " {} ", opt->String());

			renderNum++;
		}

		for(auto& tex : textuals) {
			xenomods::debug::drawFontFmtShadow(pnt.x, pnt.y += fontHeight, tex.color.a > 0 ? tex.color : Menu::COLOR_TEXTUAL, " {} ", tex.text);
		}

	}

	Section* Section::RegisterSection(const std::string& key, const std::string& display) {
		auto sec = &subsections.emplace_back(key, display);
		sec->parent = this;
		return sec;
	}

	void Section::RegisterTextual(const std::string& text, const mm::Col4& color) {
		textuals.emplace_back(text, color);
	}

} // namespace xenomods