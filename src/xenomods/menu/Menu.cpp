// Created by block on 5/28/23.

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_xeno.h>

#include <skylaunch/hookng/Hooks.hpp>
#include <xenomods/DebugWrappers.hpp>
#include <xenomods/HidInput.hpp>
#include <xenomods/ImGuiExtensions.hpp>
#include <xenomods/Logger.hpp>
#include <xenomods/NnFile.hpp>
#include <xenomods/State.hpp>
#include <xenomods/Utils.hpp>
#include <xenomods/Version.hpp>
#include <xenomods/menu/Menu.hpp>
#include <xenomods/menu/Themes.hpp>

#include "helpers/InputHelper.h"

namespace xenomods {

	struct NvnBootstrapHook : skylaunch::hook::Trampoline<NvnBootstrapHook> {
		static void* Hook(const char* name) {
			return imgui_xeno_bootstrap_hook(name, reinterpret_cast<OrigNvnBootstrap>(Backup()));
		}
	};

	void ImGuiPreInitCallback() {
		ImGuiIO& io = ImGui::GetIO();

		// add as much as we reasonably can from the loaded fonts
		static const ImWchar ranges[] = {
			0x0020, 0x00FF, // Basic Latin + Latin Supplement
			0x0102, 0x0103, // Vietnamese
			0x0110, 0x0111, // Vietnamese
			0x0128, 0x0129, // Vietnamese
			0x0168, 0x0169, // Vietnamese
			0x01A0, 0x01A1, // Vietnamese
			0x01AF, 0x01B0, // Vietnamese
			0x0370, 0x03FF, // Greek and Coptic
			0x2000, 0x206F, // General Punctuation
			0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
			0x3131, 0x3163, // Korean alphabets
			0x31F0, 0x31FF, // Katakana Phonetic Extensions
			0x4e00, 0x9FAF, // CJK Ideograms
			0xAC00, 0xD7A3, // Korean characters
			0xFF00, 0xFFEF, // Half-width characters
			0xFFFD, 0xFFFD, // Invalid
			0,
		};

		for(std::pair<std::string, float> thingy : GetState().config.menuFonts) {
			std::string path = thingy.first;

			if (!path.starts_with("sd:"))
				path = XENOMODS_CONFIG_PATH "/fonts/" + path;

			// check that the file exists first
			xenomods::NnFile file(path, nn::fs::OpenMode_Read);

			if(file) {
				file.Close();
				//g_Logger->LogDebug("{} at {}px", path, thingy.second);
				io.Fonts->AddFontFromFileTTF(path.c_str(), thingy.second, nullptr, &ranges[0]);
			}
		}
		io.Fonts->AddFontDefault();

		g_Menu->SetTheme(GetState().config.menuTheme);

		// I am fine hardcoding these for now. In the future, if we make support
		// for full file-based theming, make sure we can do colors and style.
		ImGuiStyle& style = ImGui::GetStyle();

		style.WindowPadding = ImVec2(6,6);
		style.FramePadding = ImVec2(2,1);
		style.ItemSpacing = ImVec2(8,3);
		style.ItemInnerSpacing = ImVec2(3,4);

		style.ScrollbarSize = 16;
		style.ScrollbarRounding = 0;
	}

	void Section_State() {
		ImGui::PushItemWidth(ImGui::GetFrameHeight() * 10.f);

		if(imguiext::EnumComboBox("Menu Theme", &GetState().config.menuTheme))
			g_Menu->SetTheme(GetState().config.menuTheme);

		// copied from the demo
		ImGuiIO& io = ImGui::GetIO();
		ImFont* font_current = ImGui::GetFont();
		if(ImGui::BeginCombo("Menu Font", font_current->GetDebugName())) {
			for(ImFont* font : io.Fonts->Fonts) {
				ImGui::PushID((void*)font);
				ImGui::PushFont(font);
				if(ImGui::Selectable(font->GetDebugName(), font == font_current))
					io.FontDefault = font;
				ImGui::PopFont();
				ImGui::PopID();
			}
			ImGui::EndCombo();
		}

		ImGui::PopItemWidth();

		if(ImGui::Button("Reload config/BDAT overrides"))
			XenomodsState::ReloadConfig();

		if (ImGui::Button("Toggle Log"))
			g_Menu->ToggleLog();
#if _DEBUG
		ImGui::SameLine();
		if (ImGui::Button("Logger Test"))
			g_Logger->LoggerTest();
#endif

		ImGui::Separator();
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
				about_executable = fmt::format("Game executable {}", version::RuntimeBuildRevision());
			else
				about_executable = fmt::format("Game executable version {}", version::RuntimeBuildRevision());
		}

		ImGui::TextUnformatted(version::XenomodsFullVersion());
		ImGui::TextUnformatted(about_build.c_str());
		ImGui::TextUnformatted(about_runtime.c_str());
		ImGui::TextUnformatted(about_executable.c_str());

		static bool showImGuiAbout = false;
		if (ImGui::SmallButton("About Dear ImGui"))
			showImGuiAbout = true;
		if (showImGuiAbout)
			ImGui::ShowAboutWindow(&showImGuiAbout);
	}

	void Menu::Initialize() {
		// imgui-xeno initialization
		NvnBootstrapHook::HookAt("nvnBootstrapLoader");
		imgui_xeno_add_on_pre_init(&ImGuiPreInitCallback);
		imgui_xeno_init(nullptr, &Render);

		auto modules = RegisterSection("modules", "Modules");

		auto state = RegisterSection("state", "State");
		state->RegisterRenderCallback(&Section_State);

		auto about = RegisterSection("about", "About");
		about->RegisterRenderCallback(&Section_About);
	}

	static double lastUpdateSeconds = 0;
	static double lastUpdateDiff = 0;
	void Menu::Update(HidInput* input) {
		InputHelper::setPort(input->padId);
		InputHelper::toggleInput = g_Menu->IsOpen();

		auto seconds = nn::os::GetSystemTick()/19200000.;
		lastUpdateDiff = seconds - lastUpdateSeconds;
		lastUpdateSeconds = seconds;
	}

	void Menu::Render() {
		if (!g_Menu->IsOpen())
			return;

		if (g_Menu->logOpen)
			g_Menu->Log.Draw(&g_Menu->logOpen);

		static bool test = false;
		static bool show_demo = false;
		if(ImGui::BeginMainMenuBar()) {
			for(Section* sec : g_Menu->sections) {
				if (sec == nullptr)
					continue;

				if(ImGui::BeginMenu(sec->GetName().c_str())) {
					ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false);
					sec->Render();
					ImGui::PopItemFlag();
					ImGui::EndMenu();
				}
			}
#if _DEBUG
			ImGui::MenuItem("ImGui Demo", "", &show_demo);
#endif

			ImGui::TextDisabled("%s", version::XenomodsVersion());
			ImGui::TextDisabled("FPS: %d (%.4fms)", (int)(1.f/lastUpdateDiff), lastUpdateDiff);
			ImGui::EndMainMenuBar();
		}
#if _DEBUG
		if(show_demo) {
			ImGui::ShowDemoWindow();
		}
#endif

		for(auto func : g_Menu->callbacks) {
			func();
		}
	}

	Menu::Theme Menu::SetTheme(Theme theme) {
		if (ImGui::GetCurrentContext() == nullptr)
			return Theme::Auto;

		Theme curTheme = theme;

		if (curTheme == Theme::Auto) {
#if XENOMODS_CODENAME(bfsw)
			curTheme = Theme::Titans;
#elif XENOMODS_OLD_ENGINE
			curTheme = Theme::Alrest;
#elif XENOMODS_CODENAME(bf3)
			curTheme = Theme::Aionios;
#endif
		}

		switch (curTheme) {
			case Theme::Titans: ImGuiStyleColorsXB1(); break;
			case Theme::Alrest: ImGuiStyleColorsXB2(); break;
			case Theme::Aionios: ImGuiStyleColorsXB3(); break;
			case Theme::ImGuiDark: ImGui::StyleColorsDark(); break;
			case Theme::ImGuiLight: ImGui::StyleColorsLight(); break;
			case Theme::ImGuiClassic: ImGui::StyleColorsClassic(); break;
			default: ImGui::StyleColorsDark(); return Theme::ImGuiDark;
		}

		return curTheme;
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

	void Menu::RegisterRenderCallback(void (*func)()) {
		if (func != nullptr)
			callbacks.push_back(func);
	}

	// The menu instance.
	static Menu menu;
	Menu* g_Menu = &menu;

} // namespace xenomods