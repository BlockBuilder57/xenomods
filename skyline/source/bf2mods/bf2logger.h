#ifndef BF2MODS_LOGGER_H
#define BF2MODS_LOGGER_H

#include <array>
#include <cstdarg>
#include <sstream>
#include <string>
#include <vector>

namespace bf2mods {

	/**
	 * Debug console/logger thing.
	 * This shows up on the physical screen as well as
	 * the Skyline logger console.
	 */
	struct Logger {
		Logger();

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
			OopsOuchMyBonesTheyHaveFallenHelpNoNoNONONO
		};

		/**
		 * Log a unformatted message to all sinks.
		 */
		void LogMessage(Severity severity, const std::string& message);

		inline void LogMessage(Severity severity, const char* fmt, ...) {
			//std::ostringstream ss;
			char buffer[128];

			va_list args;
			va_start(args, fmt);
			vsprintf(buffer, fmt, args);
			va_end(args);

			std::string strBuf(buffer);

			// fold expression using comma operator
			//((ss << std::forward<Args>(args)), ...);

			//LogMessage(ss.str());
			LogMessage(severity, strBuf);
		}

		template<class ...Args>
		inline void LogDebug(const char* fmt, Args... args) {
			LogMessage(Severity::Debug, fmt, std::forward<Args>(args)...);
		}
		template<class ...Args>
		inline void LogInfo(const char* fmt, Args... args) {
			LogMessage(Severity::Info, fmt, std::forward<Args>(args)...);
		}
		template<class ...Args>
		inline void LogWarning(const char* fmt, Args... args) {
			LogMessage(Severity::Warning, fmt, std::forward<Args>(args)...);
		}
		template<class ...Args>
		inline void LogError(const char* fmt, Args... args) {
			LogMessage(Severity::Error, fmt, std::forward<Args>(args)...);
		}
		template<class ...Args>
		inline void LogFatal(const char* fmt, Args... args) {
			LogMessage(Severity::OopsOuchMyBonesTheyHaveFallenHelpNoNoNONONO, fmt, std::forward<Args>(args)...);
		}

		/**
		 * Draw the current contents of the log buffer.
		 * Updates lifetime.
		 */
		void Draw();

		/**
		 * Get whether Debug severity messages should be
		 * printed to the physical screen.
		 */
		bool GetDebugEnabled() const;

		/**
		 * Set whether Debug severity messages should be
		 * printed to the physical screen.
		 *
		 * \param[in] debug_enabled Whether or not to enable Debug severity messages.
		 */
		void SetDebugEnabled(bool debug_enabled);

	   private:
		/**
		 * A message.
		 */
		struct LoggerMessage {
			std::string text;
			std::uint16_t lifetime;
			Severity severity;
		};


		/**
		 * The max amount of logger lines.
		 * Cause we don't hardcode around here.
		 */
		constexpr static auto MAX_LINES = 8;
		/**
		 * The lifetime in frames of logger lines.
		 * Xenoblade runs at 30fps (or at least tries to) and 5 seconds sounds like a good length.
		 */
		constexpr static auto LINE_LIFETIME = 5*30;
		/**
		 * When the alpha fadeout of the line starts.
		 */
		constexpr static auto FADEOUT_START = 15;

		/**
		 * Draw a message.
		 */
		void DrawInternal(std::size_t index, std::uint16_t x, std::uint16_t y) const;

		/**
		 * Add a message.
		 */
		void AddMessageInternal(Severity severity, const std::string& message);

		std::vector<LoggerMessage> lines;

		bool debug_enabled = false;
	};

	extern Logger* g_Logger;

} // namespace bf2mods

#endif //BF2MODS_LOGGER_H
