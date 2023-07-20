// Created by block on 5/28/23.

#include <imgui.h>
#include <imgui_xeno.h>

#include <skylaunch/hookng/Hooks.hpp>
#include <xenomods/DebugWrappers.hpp>
#include <xenomods/HidInput.hpp>
#include <xenomods/Logger.hpp>
#include <xenomods/State.hpp>
#include <xenomods/Utils.hpp>
#include <xenomods/Version.hpp>
#include <xenomods/menu/Menu.hpp>

//#include "helpers/InputHelper.h"

namespace xenomods {

	struct NvnBootstrapHook : skylaunch::hook::Trampoline<NvnBootstrapHook> {
		static void* Hook(const char* name) {
			return imgui_xeno_bootstrap_hook(name, reinterpret_cast<OrigNvnBootstrap>(Backup()));
			//return Orig(name);
		}
	};

	void ImGuiInitCallback() {}

	void Section_State() {
		if (ImGui::Button("Reload config/BDAT overrides"))
			XenomodsState::ReloadConfig();
	}

	std::string about_build {};
	std::string about_runtime {};
	std::string about_executable {};
	void Section_About() {
		if (about_build.empty())
			about_build = fmt::format("Compiled on {}", version::BuildTimestamp());
		if (about_runtime.empty())
			about_runtime = fmt::format("Currently running {} ({:c}) version {}", version::RuntimeGame(), version::RuntimeGame(), version::RuntimeVersion());
		if (about_executable.empty()) {
			if(std::string_view(version::RuntimeBuildRevision()).starts_with("Rev"))
				about_executable = fmt::format("Executable {}", version::RuntimeBuildRevision());
			else
				about_executable = fmt::format("Executable version {}", version::RuntimeBuildRevision());
		}

		ImGui::TextUnformatted(about_build.c_str());
		ImGui::TextUnformatted(about_runtime.c_str());
		ImGui::TextUnformatted(about_executable.c_str());
	}

	void Menu::Initialize() {
		NvnBootstrapHook::HookAt("nvnBootstrapLoader");
		imgui_xeno_init(&ImGuiInitCallback, &Render);

		auto modules = RegisterSection("modules", "Modules");

		auto state = RegisterSection("state", "State");
		state->RegisterCallback(&Section_State);

		auto about = RegisterSection("about", "About");
		about->RegisterCallback(&Section_About);
	}

	void Menu::Update(HidInput* input) {
		//InputHelper::setPort(input->padId);
	}

	void Menu::Render() {
		if (!g_Menu->IsOpen())
			return;

		static bool test = false;
		static bool show_demo = false;
		if(ImGui::BeginMainMenuBar()) {
			for(Section* sec : g_Menu->sections) {
				if (sec == nullptr)
					continue;

				if(ImGui::BeginMenu(sec->GetName().c_str())) {
					sec->Render();
					ImGui::EndMenu();
				}
			}
			ImGui::MenuItem("Imgui demo", "", &show_demo);

			ImGui::TextDisabled("%s", version::BuildXenomodsVersion());
			ImGui::EndMainMenuBar();
		}
		if(show_demo) {
			ImGui::ShowDemoWindow();
		}
	}

	Section* FindSectionRecurse(Section* section, const std::string& key) {
		if(section == nullptr)
			return nullptr;

		if(section->GetKey() == key)
			return section;

		// not us, lets check our subsections
		for(auto& sec : *section->GetSubsections()) {
			if(sec->GetKey() == key)
				return sec;

			// not this subsection, check its subsections
			auto recurse = FindSectionRecurse(sec, key);
			if(recurse != nullptr)
				return recurse;
		}

		return nullptr;
	}
	Section* Menu::FindSection(const std::string& key) {
		for(auto& sec : sections) {
			auto recurse = FindSectionRecurse(sec, key);
			if(recurse != nullptr)
				return recurse;
		}

		return nullptr;
	}

	Section* Menu::RegisterSection(const std::string& key, const std::string& display) {
		auto sec = new Section(key, display);
		sections.push_back(sec);
		return sec;
	}

	// The menu instance.
	static Menu menu;
	Menu* g_Menu = &menu;

} // namespace xenomods