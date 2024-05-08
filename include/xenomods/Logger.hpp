#pragma once

#include <array>
#include <cstdarg>
#include <sstream>
#include <string>
#include <vector>

#include "Utils.hpp"
#include "fmt/format.h"
#include "xenomods/engine/fw/UpdateInfo.hpp"
#include "xenomods/engine/mm/MathTypes.hpp"

namespace xenomods {

	/**
	 * Debug console/logger thing.
	 * This shows up on the physical screen as well as
	 * the Skyline TCP console.
	 */
	struct Logger {
		Logger();

		XENOMODS_DISALLOW_COPY(Logger, "only one instance of the logger is allowed");
		XENOMODS_DISALLOW_MOVE(Logger, "only one instance of the logger is allowed");

		/**
		 * Severity enumeration.
		 */
		enum class Severity : std::uint8_t {
			Debug,
			Info,
			Warning,
			Error,
			/**
			 * Something terribly wrong has happened.
			 * May precede a crash.
			 */
			Fatal
		};

		void VLogMessage(Severity severity, fmt::string_view format, fmt::format_args args);

		template<class FormatString, typename... Args>
		inline void LogMessage(Severity severity, const FormatString& format, Args&&... args) {
			VLogMessage(severity, format, fmt::make_format_args(args...));
		}

		template<class... Args>
		inline void LogDebug(fmt::string_view fmt, Args... args) {
			LogMessage(Severity::Debug, fmt, std::forward<Args>(args)...);
		}

		template<class... Args>
		inline void LogInfo(fmt::string_view fmt, Args... args) {
			LogMessage(Severity::Info, fmt, std::forward<Args>(args)...);
		}

		template<class... Args>
		inline void LogWarning(fmt::string_view fmt, Args... args) {
			LogMessage(Severity::Warning, fmt, std::forward<Args>(args)...);
		}

		template<class... Args>
		inline void LogError(fmt::string_view fmt, Args... args) {
			LogMessage(Severity::Error, fmt, std::forward<Args>(args)...);
		}

		template<class... Args>
		inline void LogFatal(fmt::string_view fmt, Args... args) {
			LogMessage(Severity::Fatal, fmt, std::forward<Args>(args)...);
		}

		void VToastMessage(std::string_view category, Severity severity, fmt::string_view format, fmt::format_args args);

		template<class FormatString, typename... Args>
		inline void ToastMessage(std::string_view group, Severity severity, const FormatString& format, Args&&... args) {
			VToastMessage(group, severity, format, fmt::make_format_args(args...));
		}

		template<class... Args>
		inline void ToastDebug(std::string_view group, fmt::string_view fmt, Args... args) {
			ToastMessage(group, Severity::Debug, fmt, std::forward<Args>(args)...);
		}

		template<class... Args>
		inline void ToastInfo(std::string_view group, fmt::string_view fmt, Args... args) {
			ToastMessage(group, Severity::Info, fmt, std::forward<Args>(args)...);
		}

		template<class... Args>
		inline void ToastWarning(std::string_view group, fmt::string_view fmt, Args... args) {
			ToastMessage(group, Severity::Warning, fmt, std::forward<Args>(args)...);
		}

		template<class... Args>
		inline void ToastError(std::string_view group, fmt::string_view fmt, Args... args) {
			ToastMessage(group, Severity::Error, fmt, std::forward<Args>(args)...);
		}

		template<class... Args>
		inline void ToastFatal(std::string_view group, fmt::string_view fmt, Args... args) {
			ToastMessage(group, Severity::Fatal, fmt, std::forward<Args>(args)...);
		}

		/**
		 * Performs a logger test.
		 */
		void LoggerTest();

		/**
		 * Draw all current logger lines (toasts and messages.)
		 */
		void Draw(fw::UpdateInfo* updateInfo);

		/**
		 * Draw only current message lines.
		 */
		void DrawMessages(fw::UpdateInfo* updateInfo);
		/**
		 * Draw only current toast lines.
		 */
		void DrawToasts(fw::UpdateInfo* updateInfo);

		/**
		 * Get the current level of severity messages will be
		 * printed at.
		 */
		[[nodiscard]] Severity GetLoggingLevel() const;

		/**
		 * Sets the level of severity for messages to be
		 * printed to the screen.
		 *
		 * \param[in] debug_enabled Whether or not to enable Debug severity messages.
		 */
		void SetLoggingLevel(Severity level);

		/**
		 * Returns a Col4 representing the color of the given severity.
		 *
		 * \param[in] severity The type of logger severity.
		 * \return A Col4 with the proper color.
		 */
		static mm::Col4 ColorForSeverity(const Logger::Severity& severity);

	   private:
		/**
		 * A message.
		 */
		struct LoggerMessage {
			std::string text;
			std::string group;
			float lifetime {};
			Severity severity {};
		};

		/**
		 * The max amount of logger lines.
		 * Cause we don't hardcode around here.
		 */
		constexpr static auto MAX_LINES = 8;
		/**
		 * The lifetime in frames of logger lines.
		 */
		constexpr static auto LINE_LIFETIME = 5.f;
		constexpr static auto TOAST_LIFETIME = 2.f;
		/**
		 * When the alpha fadeout of the line starts.
		 */
		constexpr static auto FADEOUT_START = 0.5f;

		/**
		 * Draw a message.
		 */
		void DrawInternal(LoggerMessage& msg, std::uint16_t x, std::uint16_t y, bool showSeverity = true) const;

		/**
		 * Add a message.
		 */
		void AddMessageInternal(Severity severity, const std::string& message);

		/**
		 * Add a toast message.
		 */
		void AddToastInternal(const std::string& group, Severity severity, const std::string& message);

		std::vector<LoggerMessage> lines {};
		std::vector<LoggerMessage> toastLines {};
		Severity loggingLevel = Severity::Info;
	};

	extern Logger* g_Logger;

} // namespace xenomods

template<>
struct fmt::formatter<xenomods::Logger::Severity> : fmt::formatter<std::string_view> {
	template<typename FormatContext>
	inline auto format(xenomods::Logger::Severity sev, FormatContext& ctx) {
		using enum xenomods::Logger::Severity;
		std::string_view name = "unknown";

		switch(sev) {
			case Debug: name = "Debug"; break;
			case Info: name = "Info"; break;
			case Warning: name = "Warning"; break;
			case Error: name = "Error"; break;
			case Fatal: name = "Fatal"; break;
		};

		return formatter<std::string_view>::format(name, ctx);
	}
};
