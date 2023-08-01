// Created by block on 5/28/23.

#include "xenomods/menu/Section.hpp"

#include <xenomods/DebugWrappers.hpp>
#include <xenomods/Logger.hpp>
#include <xenomods/menu/Menu.hpp>

#include <imgui.h>

namespace xenomods {

	Section::Section(const std::string& key, const std::string& display)
		: key(key),
		  display(display),
		  subsections() {
	}

	void Section::Render() {
		for(auto sub : subsections) {
			if(ImGui::BeginMenu(sub->GetName().c_str())) {
				sub->Render();
				ImGui::EndMenu();
			}
		}

		for(auto func : callbacks) {
			func();
		}
	}

	Section* Section::RegisterSection(const std::string& key, const std::string& display) {
		auto sec = new Section(key, display);
		sec->parent = this;
        subsections.push_back(sec);
		return sec;
	}

	void Section::RegisterRenderCallback(void (*func)()) {
		if (func != nullptr)
			callbacks.push_back(func);
	}

} // namespace xenomods