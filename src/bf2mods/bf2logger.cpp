#include "bf2logger.hpp"

#include <bf2mods/fw/debug.hpp>

#include "skyline/logger/Logger.hpp"

namespace bf2mods {

	template<>
	struct Prettyprinter<Logger::Severity> {
		inline static std::string format(const Logger::Severity& severity) {
			std::stringstream ss;
			switch(severity) {
				case Logger::Severity::Debug:
					return "Debug";
				case Logger::Severity::Info:
					return "Info";
				case Logger::Severity::Warning:
					return "Warning";
				case Logger::Severity::Error:
					return "Error";
				case Logger::Severity::Fatal:
					return "Fatal";
				default:
					assert(false && "You shouldn't get here!!!");
			}
		}

		inline static std::string_view type_name() {
			return "Logger::Severity";
		}
	};

	static mm::Col4 ColorForSeverity(const Logger::Severity& severity) {
		switch(severity) {
			case Logger::Severity::Debug:
				return { 0.25f, 0.5f, 1.f, 1.f };
			case Logger::Severity::Info:
				return mm::Col4::White;
			case Logger::Severity::Warning:
				return { 1.f, 1.f, 0.f, 1.f };
			case Logger::Severity::Error:
				return { 1.f, 0.f, 0.f, 1.f };
			case Logger::Severity::Fatal:
				return { 1.f, 0.f, 1.f, 1.f };
			default:
				assert(false && "You shouldn't get here!!!");
		};
	}

	Logger::Logger() {
		// Allocate space for logger messages upfront,
		// so the logger doesn't actually allocate memory while in use.
		lines.reserve(Logger::MAX_LINES);

#if _DEBUG
		SetDebugEnabled(true);
#endif
	}

	bool Logger::GetDebugEnabled() const {
		return debug_enabled;
	}

	void Logger::SetDebugEnabled(bool debug_enabled) {
		this->debug_enabled = debug_enabled;
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
			if(--msg.lifetime > 0)
				DrawInternal(msg, 5, 5 + (i * 16));
			else if(!lines.empty())
				// erase the current index but decrement i so we try again with the next one
				lines.erase(lines.begin() + i--);
		}
	}

	void Logger::DrawInternal(LoggerMessage& msg, std::uint16_t x, std::uint16_t y) const {
		mm::Col4 colMain = ColorForSeverity(msg.severity);
		mm::Col4 colBack = mm::Col4::Black;

		if(msg.lifetime <= 0) {
			// this shouldn't happen but better to be safe
			colMain.a = 0.f;
			colBack.a = 0.f;
		} else if(msg.lifetime <= FADEOUT_START) {
			// make the text fade out before it gets removed
			colMain.a = msg.lifetime / (float)FADEOUT_START;
			colBack.a = msg.lifetime / (float)FADEOUT_START;
		}

		fw::debug::drawFont(x + 1, y + 1, colBack, "[%s] %s", format(msg.severity).c_str(), msg.text.c_str());
		fw::debug::drawFont(x, y, colMain, "[%s] %s", format(msg.severity).c_str(), msg.text.c_str());
	}

	void Logger::AddMessageInternal(Severity severity, const std::string& message) {
		// Adapted from the Source engine

		if(lines.size() >= Logger::MAX_LINES) {
			// erase the oldest message
			lines.erase(lines.begin());
		}

		lines.push_back({ .text = message,
						  .lifetime = Logger::LINE_LIFETIME,
						  .severity = severity });
	}

	// The logger instance.
	static Logger logger;
	Logger* g_Logger = &logger;
} // namespace bf2mods