#include "xenomods/Logger.hpp"

#include <nn/diag.h>

#include "skylaunch/logger/Logger.hpp"
#include "xenomods/DebugWrappers.hpp"

namespace xenomods {

	static mm::Col4 ColorForSeverity(const Logger::Severity& severity) {
		switch(severity) {
			case Logger::Severity::Debug:
				return { 0.25f, 0.5f, 1.f, 1.f };
			case Logger::Severity::Info:
				return mm::Col4::white;
			case Logger::Severity::Warning:
				return mm::Col4::yellow;
			case Logger::Severity::Error:
				return mm::Col4::red;
			case Logger::Severity::Fatal:
				return mm::Col4::magenta;
		};
		assert(false && "You shouldn't get here!!!");
		return mm::Col4::zero;
	}

	Logger::Logger() {
		// Allocate space for logger messages upfront,
		// so the logger doesn't actually allocate memory while in use.
		lines.reserve(Logger::MAX_LINES);
		toastLines.reserve(Logger::MAX_LINES);

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

	void Logger::VLogMessage(Severity severity, fmt::string_view format, fmt::format_args args) {
		auto formatted = fmt::vformat(format, args);

		// by god I wish I didn't have to deal with this
		skylaunch::logger::s_Instance->LogFormat("[xenomods|%s] %s", fmt::format("{}", severity).c_str(), formatted.c_str());

		NN_DIAG_LOG(nn::diag::LogSeverity::Info, "[xenomods|%s] %s", fmt::format("{}", severity).c_str(), formatted.c_str());

		// Don't post Debug severity messages if we shouldn't.
		if(severity == Logger::Severity::Debug && !GetDebugEnabled())
			return;

		AddMessageInternal(severity, formatted);
	}

	void Logger::VToastMessage(std::string_view group, Severity severity, fmt::string_view format, fmt::format_args args) {
		auto formatted = fmt::vformat(format, args);

		// by god I wish I didn't have to deal with this
		skylaunch::logger::s_Instance->LogFormat("[xenomods|%s~%s] %s", group.data(), fmt::format("{}", severity).c_str(), formatted.c_str());

		NN_DIAG_LOG(nn::diag::LogSeverity::Info, "[xenomods|%s~%s] %s", group.data(), fmt::format("{}", severity).c_str(), formatted.c_str());

		// Don't post Debug severity messages if we shouldn't.
		if(severity == Logger::Severity::Debug && !GetDebugEnabled())
			return;

		AddToastInternal(std::string(group), severity, formatted);
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

		auto validToasts = 0;
		for(std::size_t i = 0; i < toastLines.size(); ++i) {
			auto& msg = toastLines[i];

			// check lifetime greater than 0, but also decrement it for next time
			if(--msg.lifetime > 0) {
				// we're right-aligned, we need the width
				int width = fw::debug::drawFontFmtGetWidth("{}", msg.text);
				DrawInternal(msg, 1280 - width - 5, 5 + (validToasts * 16), false);
				validToasts++;
			} else if(!lines.empty())
				// erase the current index but decrement i so we try again with the next one
				toastLines.erase(toastLines.begin() + i--);
		}
	}

	void Logger::DrawInternal(LoggerMessage& msg, std::uint16_t x, std::uint16_t y, bool showSeverity) const {
		mm::Col4 colMain = ColorForSeverity(msg.severity);

		if(msg.lifetime <= 0) {
			// this shouldn't happen but better to be safe
			colMain.a = 0.f;
		} else if(msg.lifetime <= FADEOUT_START) {
			// make the text fade out before it gets removed
			colMain.a = msg.lifetime / (float)FADEOUT_START;
		}

		// these are passed as format arguments to avoid fuckery in the message
		if(showSeverity)
			fw::debug::drawFontFmtShadow(x, y, colMain, "[{}] {}", msg.severity, msg.text);
		else
			fw::debug::drawFontFmtShadow(x, y, colMain, "{}", msg.text);
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

	void Logger::AddToastInternal(const std::string& group, Severity severity, const std::string& message) {
		if(toastLines.size() >= Logger::MAX_LINES) {
			// erase the oldest message
			toastLines.erase(toastLines.begin());
		}

		LoggerMessage* groupMessage = nullptr;
		for(auto& line : toastLines) {
			/*if (groupMessage != nullptr) {
				LogError("Multiple toasts with same group!!");
				toastLines.clear();
				break;
			}*/
			if(line.group == group) {
				groupMessage = &line;
				break;
			}
		}

		if(groupMessage != nullptr) {
			// we already have a message in this group, update its properties
			groupMessage->text = message;
			groupMessage->severity = severity;
			groupMessage->lifetime = Logger::TOAST_LIFETIME;
		} else {
			// create a new message!
			toastLines.push_back({ .text = message,
									.group = group,
									.lifetime = Logger::TOAST_LIFETIME,
									.severity = severity });
		}
	}

	// The logger instance.
	static Logger logger;
	Logger* g_Logger = &logger;

} // namespace xenomods