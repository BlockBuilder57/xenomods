// Created by block on 5/28/23.

#include "xenomods/menu/Section.hpp"

#include <xenomods/DebugWrappers.hpp>
#include <xenomods/Logger.hpp>
#include <xenomods/menu/Menu.hpp>

namespace xenomods {

	Section::Section(const std::string& key, const std::string& display)
		: key(key),
		  display(display),
		  subsections(),
		  options(),
		  textuals() {
	}

	void Section::Update(HidInput* input) {
		SavedIndex = g_Menu->curIndex;

		if(g_Menu->curIndex < subsections.size()) {
			// we're within the subsections, clear the current option
			curOption = nullptr;
		} else if(g_Menu->curIndex - subsections.size() < options.size()) {
			// we're in the options
			if(curOption != nullptr) {
				if(curOption->IsSelected()) {
					// the option is selected, let's update it
					if(curOption->Update(input))
						curOption->Callback();
				} else {
					// it wants to be deselected
					curOption = nullptr;
				}
			}
		}
	}

	void Section::PerformSelect() {
		if(g_Menu->curIndex < subsections.size()) {
			// we're within the subsections, select the currently highlighted section
			g_Menu->curSection = subsections[g_Menu->curIndex];
			g_Menu->curIndex = g_Menu->curSection->SavedIndex;
		} else if(g_Menu->curIndex - subsections.size() < options.size()) {
			// we're in the options, and we want to select this one
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
			if (sec == nullptr)
				continue;

			if(renderNum == g_Menu->curIndex)
				xenomods::debug::drawFontFmtShadow(pnt.x, pnt.y += fontHeight, g_Menu->pressSelect ? Menu::COLOR_HIGHLIGHT : Menu::COLOR_SECTION, ">{} ", sec->GetName());
			else
				xenomods::debug::drawFontFmtShadow(pnt.x, pnt.y += fontHeight, Menu::COLOR_SECTION, " {} ", sec->GetName());

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
			xenomods::debug::drawFontFmtShadow(pnt.x, pnt.y += fontHeight, tex.Color().a > 0 ? tex.Color() : Menu::COLOR_TEXTUAL, " {} ", tex.String());
		}
	}

	Section* Section::RegisterSection(const std::string& key, const std::string& display) {
		auto sec = new Section(key, display);
		sec->parent = this;
        subsections.push_back(sec);
		return sec;
	}

	void Section::RegisterTextual(const std::string& text, const mm::Col4& color, std::string (*callback)()) {
		textuals.emplace_back(text, color, callback);
	}

} // namespace xenomods