// Created by block on 8/28/23.

#include <xenomods/ImGuiExtensions.hpp>

// That sign won't stop me because I can't read!
#include <imgui_internal.h>

namespace xenomods::imguiext {

	// Most of this is from ImGui::InputScalar
	bool InputFloatExt(const char* label, float* v, float step, float step_fast, float step_mult, const char* format, ImGuiInputTextFlags flags) {
		flags |= ImGuiInputTextFlags_CharsScientific;

		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if(window->SkipItems)
			return false;

		ImGuiContext& g = *ImGui::GetCurrentContext();
		ImGuiStyle& style = g.Style;

		if(format == NULL)
			format = ImGui::DataTypeGetInfo(ImGuiDataType_Float)->PrintFmt;

		char buf[64];
		ImGui::DataTypeFormatString(buf, IM_ARRAYSIZE(buf), ImGuiDataType_Float, v, format);

		flags |= ImGuiInputTextFlags_AutoSelectAll; // We call MarkItemEdited() ourselves by comparing the actual data rather than the string.

		bool value_changed = false;
		const float button_size = ImGui::GetFrameHeight();

		ImGui::BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
		ImGui::PushID(label);
		ImGui::SetNextItemWidth(ImMax(1.0f, ImGui::CalcItemWidth() - (button_size + style.ItemInnerSpacing.x) * 4));
		if(ImGui::InputText("", buf, IM_ARRAYSIZE(buf), flags)) // PushId(label) + "" gives us the expected ID from outside point of view
			value_changed = ImGui::DataTypeApplyFromText(buf, ImGuiDataType_Float, v, format);
		IMGUI_TEST_ENGINE_ITEM_INFO(g.LastItemData.ID, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Inputable);

		// Step buttons
		const ImVec2 backup_frame_padding = style.FramePadding;
		style.FramePadding.x = style.FramePadding.y;

		if(flags & ImGuiInputTextFlags_ReadOnly)
			ImGui::BeginDisabled();

		ImGui::SameLine(0, style.ItemInnerSpacing.x);

		ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
		ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false);
		if(ImGui::ButtonEx("-", ImVec2(button_size, button_size))) {
			*v -= g.IO.KeyCtrl && step_fast > 0.0f ? step_fast : step;
			value_changed = true;
		}
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		if(ImGui::ButtonEx("+", ImVec2(button_size, button_size))) {
			*v += g.IO.KeyCtrl && step_fast > 0.0f ? step_fast : step;
			value_changed = true;
		}
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		if(ImGui::ButtonEx("/", ImVec2(button_size, button_size))) {
			*v /= g.IO.KeyCtrl && step_fast > 0.0f ? step_fast : step_mult;
			value_changed = true;
		}
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		if(ImGui::ButtonEx("*", ImVec2(button_size, button_size))) {
			*v *= g.IO.KeyCtrl && step_fast > 0.0f ? step_fast : step_mult;
			value_changed = true;
		}
		ImGui::PopItemFlag();
		ImGui::PopItemFlag();

		if(flags & ImGuiInputTextFlags_ReadOnly)
			ImGui::EndDisabled();

		const char* label_end = ImGui::FindRenderedTextEnd(label);
		if(label != label_end) {
			ImGui::SameLine(0, style.ItemInnerSpacing.x);
			ImGui::TextEx(label, label_end);
		}
		style.FramePadding = backup_frame_padding;

		ImGui::PopID();
		ImGui::EndGroup();

		if(value_changed)
			ImGui::MarkItemEdited(g.LastItemData.ID);

		return value_changed;
	}

}