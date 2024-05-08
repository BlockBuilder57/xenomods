// Created by block on 5/7/24.

#pragma once

#include <imgui.h>

#include <xenomods/Logger.hpp>

namespace xenomods {

	// Doing this the way ImGui recommends in the demo.
	class MenuLog {
	   public:
		MenuLog();

		void Clear();
		void Add(const Logger::Severity& severity, const std::string& text);
		void Add(const Logger::Severity& severity, const std::string_view& group, const std::string& text);

		void SaveToFile();

		void Draw(bool* open = nullptr);
	   private:
		ImGuiTextBuffer buffer {};
		ImVector<int> lineOffsets {};
		ImVector<Logger::Severity> lineSeverity {};
	};

} // namespace xenomods
