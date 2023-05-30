// Created by block on 5/28/23.

#include <skylaunch/hookng/Hooks.hpp>
#include <xenomods/DebugWrappers.hpp>
#include <xenomods/HidInput.hpp>
#include <xenomods/Logger.hpp>
#include <xenomods/State.hpp>
#include <xenomods/Utils.hpp>
#include <xenomods/Version.hpp>
#include <xenomods/menu/Menu.hpp>

#include "../modules/MenuViewer.hpp"

namespace xenomods {

	mm::Col4 Menu::COLOR_BACKGROUND = { 0, 0, 0, 0.6f };
	mm::Col4 Menu::COLOR_TITLE = { 1, 1, 1, 1 };
	mm::Col4 Menu::COLOR_SECTION = { 1, 1, 1, 1 };
	mm::Col4 Menu::COLOR_OPTION = { 0.9f, 0.9f, 0.9f, 1 };
	mm::Col4 Menu::COLOR_TEXTUAL = { 0.8f, 0.8f, 0.8f, 1 };
	mm::Col4 Menu::COLOR_HIGHLIGHT = { 0.75f, 0.75f, 1, 1 };

	void Menu::Initialize() {
		// modules
		auto modules = RegisterSection("modules", "Modules...");

		// state
		auto state = RegisterSection("state", "State...");

		state->RegisterOption<bool>(drawBackground, "Draw menu background");
		state->RegisterOption<void>("Reload config/BDAT overrides", &XenomodsState::ReloadConfig);

		// about
		auto about = RegisterSection("about", "About...");
		about->RegisterTextual(fmt::format("Compiled on {}", version::BuildTimestamp()));
		about->RegisterTextual(fmt::format("Currently running {} ({:c}) version {}", version::RuntimeGame(), version::RuntimeGame(), version::RuntimeVersion()));

		// i love grammar
		if(std::string_view(version::RuntimeBuildRevision()).starts_with("Rev"))
			about->RegisterTextual(fmt::format("Executable {}", version::RuntimeBuildRevision()));
		else
			about->RegisterTextual(fmt::format("Executable version {}", version::RuntimeBuildRevision()));
	}

	void Menu::Update(HidInput* input) {
		PollMaxIndex();

		pressSelect = false;
		pressBack = false;
		bool doSelect = false;
		bool doBack = false;

		// only update controls when no option is selected
		if(curSection == nullptr || !curSection->IsSelectingOption()) {
			if(input->InputDownStrict(Keybind::MENU_UP)) {
				curIndex--;
			} else if(input->InputDownStrict(Keybind::MENU_DOWN)) {
				curIndex++;
			}

			// wrap around
			if(curIndex >= maxIndex)
				curIndex = 0;
			if(curIndex < 0)
				curIndex = maxIndex - 1;

			pressSelect = input->InputHeldStrict(Keybind::MENU_SELECT);
			pressBack = input->InputHeldStrict(Keybind::MENU_BACK);

			if(input->InputUpStrict(Keybind::MENU_SELECT)) {
				doSelect = true;
			} else if(input->InputUpStrict(Keybind::MENU_BACK)) {
				doBack = true;
			}
		}

		if(curSection != nullptr)
			curSection->Update(input);

		Render();

		// intentionally switching sections after rendering, so we draw the selection highlight

		if(doBack) {
			if(curSection != nullptr)
				curSection = curSection->GetParent();
		} else if(doSelect) {
			if(curSection == nullptr) {
				// on root menu, can just pick
				curSection = &sections[curIndex];
			} else {
				// let the section handle stuff
				curSection->PerformSelect();
			}
		}
	}

	void Menu::Render() {
		if(drawBackground)
			xenomods::debug::drawFontBackColor(COLOR_BACKGROUND);

		const int fontHeight = xenomods::debug::drawFontGetHeight();
		const mm::Pnt<int> start { .x = 5, .y = 5 };
		mm::Pnt<int> pnt = start;

		// //xenomods 1234567~ (debug) [???]
		xenomods::debug::drawFontFmtShadow(start.x, start.y, COLOR_TITLE, "\x81\x61xenomods {}{} [{}]", version::BuildGitVersion(), version::BuildIsDebug ? " (debug)" : "", XENOMODS_CODENAME_STR);

		int renderNum = 0;

		if(curSection == nullptr) {
			// render the root sections
			for(auto& sec : sections) {
				if(renderNum == curIndex)
					xenomods::debug::drawFontFmtShadow(pnt.x, pnt.y += fontHeight, pressSelect ? COLOR_HIGHLIGHT : COLOR_SECTION, ">{} ", sec.GetName());
				else
					xenomods::debug::drawFontFmtShadow(pnt.x, pnt.y += fontHeight, COLOR_SECTION, " {} ", sec.GetName());

				renderNum++;
			}
		} else
			// render the current section
			curSection->Render(pnt);

		if(drawBackground)
			xenomods::debug::drawFontBackColor({});
	}

	void Menu::PollMaxIndex() {
		if(curSection == nullptr)
			maxIndex = sections.size();
		else
			maxIndex = curSection->GetMaxIndex();

		std::clamp(curIndex, 0, maxIndex);
	}

	Section* FindSectionRecurse(Section* section, const std::string& key) {
		if(section == nullptr)
			return nullptr;

		if(section->GetKey() == key)
			return section;

		// not us, lets check our subsections
		for(auto& sec : *section->GetSubsections()) {
			if(sec.GetKey() == key)
				return &sec;

			// not this subsection, check its subsections
			auto recurse = FindSectionRecurse(&sec, key);
			if(recurse != nullptr)
				return recurse;
		}

		return nullptr;
	}
	Section* Menu::FindSection(const std::string& key) {
		for(auto& sec : sections) {
			auto recurse = FindSectionRecurse(&sec, key);
			if(recurse != nullptr)
				return recurse;
		}

		return nullptr;
	}

	Section* Menu::RegisterSection(const std::string& key, const std::string& display) {
		return &sections.emplace_back(key, display);
	}

	// The menu instance.
	static Menu menu;
	Menu* g_Menu = &menu;

} // namespace xenomods