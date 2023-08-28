// Created by block on 8/28/23.

#pragma once

#include <imgui.h>

namespace xenomods {

	// "Titans"
	inline void ImGuiStyleColorsXB1() {
		ImGuiStyle& style = ImGui::GetStyle();

		style.Colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled]           = ImVec4(0.86f, 0.81f, 0.75f, 0.60f);
		style.Colors[ImGuiCol_WindowBg]               = ImVec4(0.24f, 0.22f, 0.17f, 0.90f);
		style.Colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		style.Colors[ImGuiCol_Border]                 = ImVec4(0.57f, 0.51f, 0.45f, 0.50f);
		style.Colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg]                = ImVec4(0.29f, 0.27f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.56f, 0.53f, 0.46f, 0.40f);
		style.Colors[ImGuiCol_FrameBgActive]          = ImVec4(0.79f, 0.76f, 0.72f, 0.67f);
		style.Colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		style.Colors[ImGuiCol_TitleBgActive]          = ImVec4(0.42f, 0.28f, 0.22f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		style.Colors[ImGuiCol_MenuBarBg]              = ImVec4(0.13f, 0.13f, 0.12f, 0.78f);
		style.Colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		style.Colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		style.Colors[ImGuiCol_CheckMark]              = ImVec4(0.90f, 0.94f, 0.30f, 0.78f);
		style.Colors[ImGuiCol_SliderGrab]             = ImVec4(0.60f, 0.57f, 0.52f, 1.00f);
		style.Colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.79f, 0.76f, 0.72f, 1.00f);
		style.Colors[ImGuiCol_Button]                 = ImVec4(0.32f, 0.30f, 0.28f, 1.00f);
		style.Colors[ImGuiCol_ButtonHovered]          = ImVec4(0.69f, 0.64f, 0.12f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive]           = ImVec4(0.91f, 0.97f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_Header]                 = ImVec4(0.80f, 0.24f, 0.24f, 0.31f);
		style.Colors[ImGuiCol_HeaderHovered]          = ImVec4(0.80f, 0.24f, 0.24f, 0.66f);
		style.Colors[ImGuiCol_HeaderActive]           = ImVec4(0.69f, 0.64f, 0.12f, 0.76f);
		style.Colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		style.Colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
		style.Colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip]             = ImVec4(0.55f, 0.27f, 0.15f, 0.20f);
		style.Colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.55f, 0.27f, 0.15f, 0.67f);
		style.Colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.69f, 0.64f, 0.12f, 0.76f);
		style.Colors[ImGuiCol_Tab]                    = ImVec4(0.29f, 0.27f, 0.24f, 0.46f);
		style.Colors[ImGuiCol_TabHovered]             = ImVec4(0.75f, 0.87f, 0.31f, 0.80f);
		style.Colors[ImGuiCol_TabActive]              = ImVec4(0.55f, 0.27f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_TabUnfocused]           = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
		style.Colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.13f, 0.26f, 0.42f, 1.00f);
		//style.Colors[ImGuiCol_DockingPreview]         = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
		//style.Colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		style.Colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		style.Colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		style.Colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		style.Colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		style.Colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		style.Colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		style.Colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	}

	// "Alrest"
	inline void ImGuiStyleColorsXB2() {
		ImGui::StyleColorsDark();
	}

	// "Aionios"
	inline void ImGuiStyleColorsXB3() {
		ImGui::StyleColorsDark();
	}

}