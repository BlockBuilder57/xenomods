#include "bf2logger.h"

#include "debug_stuff.hpp" // fw::debug
#include "skyline/logger/Logger.hpp"

namespace bf2mods {


	template<>
	struct Prettyprinter<Logger::Severity> {
		inline static std::string format(const Logger::Severity& severity) {
			std::stringstream ss;
			switch(severity) {
				case Logger::Severity::Debug:
					ss << "Debug";
					break;
				case Logger::Severity::Info:
					ss << "Info";
					break;
				case Logger::Severity::Warning:
					ss << "Warning";
					break;
				case Logger::Severity::Error:
					ss << "Error";
					break;
				case Logger::Severity::OopsOuchMyBonesTheyHaveFallenHelpNoNoNONONO:
					ss << "Fatal";
					break;
			}
			return ss.str();
		}

		inline static std::string_view type_name() { return "Logger::Severity"; }

	};

	static mm::Col4 ColorForSeverity(const Logger::Severity& severity) {
		switch(severity) {
			case Logger::Severity::Debug:
				return { 0.f, 0.f, 1.f, 1.f };
				break;
			case Logger::Severity::Info:
				return mm::Col4::White;
				break;
			case Logger::Severity::Warning:
				return { 1.f, 1.f, 0.f, 1.f };
				break;
			case Logger::Severity::Error:
				return { 1.f, 0.f, 0.f, 1.f };
				break;
			case Logger::Severity::OopsOuchMyBonesTheyHaveFallenHelpNoNoNONONO:
				return { 1.f, 0.f, 1.f, 1.f };
				break;
		};

		return mm::Col4::White;
	}

	Logger::Logger() {
		lines.resize(Logger::MAX_LINES);
	}

	void Logger::LogMessage(Severity severity, const std::string& message) {
		skyline::logger::s_Instance->LogFormat("[bf2mods Console] [%s] %s", format(severity).c_str(), message.c_str());

		// Don't post Debug severity messages if we shouldn't.
		if(severity == Logger::Severity::Debug && !GetDebugEnabled())
			return;

		AddMessageInternal(severity, message);
	}

	void Logger::Draw() {
		for(std::size_t i = 0; i < lines.size(); ++i) {
			auto& msg = lines[i];

			// check lifetime greater than 0, but also decrement it for next time
			if (msg.lifetime-- > 0)
				DrawInternal(i, 5 + 0, 5 + (i * 16));
			else
				// erase the current index but decrement i so we try again with the next one
				lines.erase(lines.begin() + i--);
		}
	}

	void Logger::DrawInternal(std::size_t index, std::uint16_t x, std::uint16_t y) const {
		mm::Col4 colMain = ColorForSeverity(lines[index].severity);
		mm::Col4 colBack = mm::Col4::Black;

		if (lines[index].lifetime == 0) {
			// this shouldn't happen but better to be safe
			colMain.a = 0.f;
			colBack.a = 0.f;
		}
		else if (lines[index].lifetime <= FADEOUT_START) {
			// make the text fade out before it gets removed
			colMain.a = lines[index].lifetime / (float)FADEOUT_START;
			colBack.a = lines[index].lifetime / (float)FADEOUT_START;
		}

		fw::debug::drawFont(x + 1, y + 1, &colBack, "[%s] %s", format(lines[index].severity).c_str(), lines[index].text.c_str());
		fw::debug::drawFont(x, y, &colMain, "[%s] %s", format(lines[index].severity).c_str(), lines[index].text.c_str());
	}

	void Logger::AddMessageInternal(Severity severity, const std::string& message) {
		// Adapted from the Source engine

		std::size_t slot = lines.size() - 1;

		if (slot < 0)
			slot = 0;

		if (slot >= 0 && lines.size() < Logger::MAX_LINES) {
			// if we have a slot and lines is not full, add one
			lines.push_back({
				.text = message,
				.lifetime = Logger::LINE_LIFETIME,
				.severity = severity
			});
		} else if (!lines.empty()) {
			// otherwise, erase the oldest one
			lines.erase(lines.begin());
		}
	}

	// The logger instance.
	static Logger logger;
	Logger* g_Logger = &logger;
} // namespace bf2mods