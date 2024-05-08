// Created by block on 5/7/24.

#include <chrono>
#include <xenomods/menu/MenuLog.hpp>

#include "xenomods/ImGuiExtensions.hpp"
#include "xenomods/NnFile.hpp"
#include "xenomods/State.hpp"

namespace xenomods {

	MenuLog::MenuLog() {
		Clear();
	}

	void MenuLog::Clear() {
		buffer.clear();
		lineOffsets.clear();
		lineSeverity.clear();
	}

	void MenuLog::Add(const Logger::Severity& severity, const std::string& text) {
		for (std::string str : xenomods::StringSplit(fmt::format("[{}] {}", severity, text).data(), "\n")) {
			buffer.append(str.data());
			lineOffsets.push_back(buffer.size());
			lineSeverity.push_back(severity);
		}
	}

	void MenuLog::Add(const Logger::Severity& severity, const std::string_view& group, const std::string& text) {
		for (std::string str : xenomods::StringSplit(fmt::format("[{}~{}] {}", severity, group.data(), text).data(), "\n")) {
			buffer.append(str.data());
			lineOffsets.push_back(buffer.size());
			lineSeverity.push_back(severity);
		}
	}

	void MenuLog::SaveToFile() {
		const std::string logpath = XENOMODS_CONFIG_PATH "/logs/";

		// there seems to be some epoch weirdness with the normal std::chrono::system_clock::now
		// nn has functionality for this, and that's what the game itself uses to get its info
		nn::time::PosixTime time {};
		nn::time::StandardUserSystemClock::GetCurrentTime(&time);
		std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> tp { std::chrono::seconds { time.time } };
		std::string timestamp = std::format("{:%F %H-%M-%S}", tp);

		xenomods::g_Logger->LogInfo("Attempting save at {}", timestamp);
		std::string path = logpath + timestamp + ".log";

		int size = buffer.size() + lineOffsets.size();

		if(!xenomods::NnFile::Preallocate(path, size)) {
			xenomods::g_Logger->LogError("Couldn't create/preallocate dump file \"{}\"", path);
			return;
		}

		xenomods::NnFile file(path, nn::fs::OpenMode_Write);

		if(!file) {
			xenomods::g_Logger->LogError("Couldn't save log to file \"{}\"", path);
		} else {
			for(int line_no = 0; line_no < lineOffsets.Size; line_no++) {
				const char* line_start = buffer.begin() + (line_no <= 0 ? 0 : lineOffsets[line_no - 1]);
				const char* line_end = buffer.begin() + lineOffsets[line_no];

				file.Write(line_start, line_end - line_start);
				file.Write("\n", 1);
			}

			file.Flush();
		}
		file.Close();
	}

	void MenuLog::Draw(bool* open) {
		ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
		if(!ImGui::Begin("Xenomods Log", open)) {
			ImGui::End();
			return;
		}

		ImGui::PushItemWidth(72.f);
		if(imguiext::EnumComboBox("Logging Level", &GetState().config.loggingLevel))
			g_Logger->SetLoggingLevel(GetState().config.loggingLevel);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if(ImGui::Button("Clear"))
			Clear();
		ImGui::SameLine();
		if(ImGui::Button("Save To File"))
			SaveToFile();

		ImGui::Separator();

		ImGui::SetNextWindowBgAlpha(0.2f);
		if(ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar)) {
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			const char* buf = buffer.begin();
			const char* buf_end = buffer.end();
			{
				// Refer to imgui_demo.cpp for details on how this works, but essentially we'd like to clip the lines
				// we actually want to see in order to avoid a massive performance penalty.
				// But because of our filtering, we can't. :(
				//ImGuiListClipper clipper;
				//clipper.Begin(lineOffsets.Size);
				//while(clipper.Step()) {
				for(int line_no = 0; line_no < lineOffsets.Size; line_no++) {
					//for(int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++) {
					const char* line_start = buf + (line_no <= 0 ? 0 : lineOffsets[line_no - 1]);
					const char* line_end = buf + lineOffsets[line_no];

					int sev = line_no;

					if(lineSeverity[sev] < xenomods::g_Logger->GetLoggingLevel())
						continue;

					mm::Col4 col = xenomods::Logger::ColorForSeverity(lineSeverity[sev]);
					ImGui::PushStyleColor(ImGuiCol_Text, { col.r, col.g, col.b, col.a });
					ImGui::TextUnformatted(line_start, line_end);
					ImGui::PopStyleColor();
				}
				//}
				//clipper.End();
			}
			ImGui::PopStyleVar();

			// Keep scroll at bottom if we've scrolled down there
			if(ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);
		}
		ImGui::EndChild();
		ImGui::End();
	}

} // namespace xenomods